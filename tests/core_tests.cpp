#include "vehicle_controls/control.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <string_view>

namespace {
int failures = 0;
void check(bool condition, const char *message) {
  if (!condition) {
    std::cerr << message << '\n';
    ++failures;
  }
}
bool near(double a, double b) { return std::abs(a - b) < 1e-9; }
} // namespace
int main(int argc, char **argv) {
  if (argc != 2) {
    return 2;
  }
  const std::string_view test(argv[1]);
  vc::Controller controller;
  vc::State state;
  vc::Reference reference{20.0, 0.002, 0.0};
  if (test == "nominal") {
    for (int tick = 0; tick < 6000; ++tick) {
      const auto u = controller.update(state, reference, 0.01);
      check(u.mode == vc::Mode::nominal, "unexpected stop");
      check(vc::predict(state, u, 0.01), "prediction failed");
    }
    check(std::abs(state.speed - reference.speed) < 0.05, "speed tracking failed");
    check(std::abs(vc::angle_difference(state.yaw, state.trailer_yaw)) < 0.1,
          "trailer articulation did not settle");
  } else if (test == "limits") {
    reference.curvature = 0.05;
    auto prior = controller.update(state, reference, 0.01);
    check(near(prior.acceleration, 1.5), "acceleration limit");
    for (int tick = 0; tick < 1000; ++tick) {
      const auto u = controller.update(state, reference, 0.01);
      check(std::abs(u.steering) <= 0.45, "steering angle limit");
      check(std::abs(u.steering - prior.steering) <= 0.003 + 1e-12, "steering rate limit");
      prior = u;
    }
    state.speed = 30.0;
    for (int tick = 0; tick < 1000; ++tick) {
      prior = controller.update(state, reference, 0.01);
    }
    check(state.speed * state.speed * std::tan(prior.steering) / 3.8 <= 2.0 + 1e-9,
          "steady-state lateral acceleration bound");
    check(near(prior.acceleration, -3.0), "braking limit");
  } else if (test == "stale") {
    reference.age = 0.1;
    check(controller.update(state, reference, 0.01).mode == vc::Mode::nominal, "timeout boundary");
    reference.age = 0.100001;
    const auto u = controller.update(state, reference, 0.01);
    check(u.mode == vc::Mode::stopping && near(u.acceleration, -3.0), "stale command stop");
  } else if (test == "invalid") {
    const double bad[] = {std::numeric_limits<double>::quiet_NaN(),
                          std::numeric_limits<double>::infinity(),
                          -std::numeric_limits<double>::infinity()};
    for (double value : bad) {
      for (int field = 0; field < 9; ++field) {
        vc::State s;
        vc::Reference r = reference;
        double dt = 0.01;
        switch (field) {
        case 0:
          s.x = value;
          break;
        case 1:
          s.y = value;
          break;
        case 2:
          s.yaw = value;
          break;
        case 3:
          s.trailer_yaw = value;
          break;
        case 4:
          s.speed = value;
          break;
        case 5:
          r.speed = value;
          break;
        case 6:
          r.curvature = value;
          break;
        case 7:
          r.age = value;
          break;
        default:
          dt = value;
          break;
        }
        vc::Controller fresh;
        const auto u = fresh.update(s, r, dt);
        check(u.mode == vc::Mode::stopping && std::isfinite(u.acceleration) &&
                  std::isfinite(u.steering),
              "non-finite input not contained");
      }
    }
    for (double dt : {-0.1, 0.0, 0.100001}) {
      vc::Controller fresh;
      check(fresh.update(state, reference, dt).mode == vc::Mode::stopping, "invalid dt");
    }
    for (double speed : {-1.0, 35.001}) {
      vc::Controller fresh;
      reference.speed = speed;
      check(fresh.update(state, reference, 0.01).mode == vc::Mode::stopping,
            "invalid target speed");
    }
  } else if (test == "articulation") {
    state.yaw = 0.60001;
    check(controller.update(state, reference, 0.01).mode == vc::Mode::stopping,
          "articulation guard");
    vc::Controller fresh;
    state.yaw = 3.13;
    state.trailer_yaw = -3.13;
    check(fresh.update(state, reference, 0.01).mode == vc::Mode::nominal, "angle wrapping");
  } else if (test == "latch") {
    reference.age = 1.0;
    const auto fault = controller.update(state, reference, 0.01);
    reference.age = 0.0;
    const auto recovered = controller.update(state, reference, 0.01);
    check(fault.mode == vc::Mode::stopping && recovered.mode == vc::Mode::stopping,
          "fault must latch until controller restart");
  } else if (test == "prediction") {
    state.speed = 10.0;
    check(vc::predict(state, {0.0, 0.0, vc::Mode::nominal}, 0.1), "straight prediction");
    check(near(state.x, 1.0) && near(state.y, 0.0) && near(state.yaw, 0.0), "straight solution");
    const auto saved = state;
    check(!vc::predict(state, {0.0, 1.0, vc::Mode::nominal}, 0.1), "invalid actuation accepted");
    check(near(state.x, saved.x) && near(state.speed, saved.speed),
          "failed prediction mutated state");
    state = {};
    state.speed = 10.0;
    check(vc::predict(state, {0.0, 0.1, vc::Mode::nominal}, 0.1), "turn prediction");
    check(near(state.yaw, std::tan(0.1) / 3.8), "bicycle yaw rate");
  } else if (test == "braking") {
    state.speed = 20.0;
    for (int tick = 0; tick < 1000; ++tick) {
      check(vc::predict(state, {-3.0, 0.0, vc::Mode::stopping}, 0.01), "braking prediction");
    }
    check(state.speed == 0.0, "braking must reach standstill");
    check(std::abs(state.x - 400.0 / 6.0) < 1e-8, "analytical braking distance");
  } else {
    return 2;
  }
  return failures == 0 ? 0 : 1;
}
