#include "vehicle_controls/control.hpp"
#include <algorithm>
#include <cmath>

namespace vc {
namespace {
constexpr double wheelbase = 3.8;
constexpr double trailer_length = 8.0;
constexpr double pi = 3.14159265358979323846;
bool finite(const State &s) noexcept {
  return std::isfinite(s.x) && std::isfinite(s.y) && std::isfinite(s.yaw) &&
         std::isfinite(s.trailer_yaw) && std::isfinite(s.speed) && s.speed >= 0.0 &&
         s.speed <= 45.0;
}
bool valid_dt(double dt) noexcept { return std::isfinite(dt) && dt > 0.0 && dt <= 0.1; }
} // namespace

double angle_difference(double a, double b) noexcept {
  return std::remainder(std::remainder(a, 2.0 * pi) - std::remainder(b, 2.0 * pi), 2.0 * pi);
}
Actuation Controller::update(const State &s, const Reference &r, double dt) noexcept {
  const bool valid = finite(s) && valid_dt(dt) && std::isfinite(r.speed) && r.speed >= 0.0 &&
                     r.speed <= 35.0 && std::isfinite(r.curvature) &&
                     std::abs(r.curvature) <= 0.05 && std::isfinite(r.age) && r.age >= 0.0 &&
                     r.age <= 0.1 && std::abs(angle_difference(s.yaw, s.trailer_yaw)) <= 0.6;
  if (!valid) {
    mode_ = Mode::stopping;
  }
  if (mode_ == Mode::stopping) {
    // Holding the last bounded steering avoids an abrupt recentering transient.
    return {-3.0, steering_, mode_};
  }
  // Curvature feedforward; speed-dependent lateral acceleration bound of 2 m/s^2.
  const double lateral_limit = std::atan(2.0 * wheelbase / std::max(s.speed * s.speed, 1.0));
  const double limit = std::min(0.45, lateral_limit);
  const double target = std::clamp(std::atan(wheelbase * r.curvature), -limit, limit);
  steering_ += std::clamp(target - steering_, -0.3 * dt, 0.3 * dt);
  return {std::clamp(0.8 * (r.speed - s.speed), -3.0, 1.5), steering_, mode_};
}
bool predict(State &s, const Actuation &u, double dt) noexcept {
  if (!finite(s) || !valid_dt(dt) || !std::isfinite(u.acceleration) || !std::isfinite(u.steering) ||
      u.acceleration < -3.0 || u.acceleration > 1.5 || std::abs(u.steering) > 0.45) {
    return false;
  }
  // Integrate only to standstill when braking; no reverse motion or artificial creep.
  const double moving_dt = u.acceleration < 0.0 ? std::min(dt, s.speed / -u.acceleration) : dt;
  const double distance = s.speed * moving_dt + 0.5 * u.acceleration * moving_dt * moving_dt;
  State next = s;
  next.x += distance * std::cos(s.yaw);
  next.y += distance * std::sin(s.yaw);
  next.yaw = angle_difference(s.yaw + distance * std::tan(u.steering) / wheelbase, 0.0);
  next.trailer_yaw = angle_difference(
      s.trailer_yaw + distance * std::sin(angle_difference(s.yaw, s.trailer_yaw)) / trailer_length,
      0.0);
  next.speed = std::max(0.0, s.speed + u.acceleration * dt);
  if (!finite(next)) {
    return false;
  }
  s = next;
  return true;
}
} // namespace vc
