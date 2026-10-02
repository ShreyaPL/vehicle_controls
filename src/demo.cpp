#include "vehicle_controls/control.hpp"
#include <cmath>
#include <iostream>
#include <string_view>

int main(int argc, char **argv) {
  const bool dropout = argc == 2 && std::string_view(argv[1]) == "--dropout";
  if (argc > 2 || (argc == 2 && !dropout)) {
    std::cerr << "Usage: truck_demo [--dropout]\n";
    return 2;
  }
  vc::Controller controller;
  vc::State state;
  std::cout << "time_s,x_m,y_m,speed_mps,yaw_rad,trailer_yaw_rad,accel_mps2,steer_rad,stopping\n";
  for (int tick = 0; tick < 6000; ++tick) {
    const double time = static_cast<double>(tick) * 0.01;
    const double age = dropout && tick >= 3000 ? static_cast<double>(tick - 3000) * 0.01 : 0.0;
    const auto command = controller.update(state, {20.0, 0.002, age}, 0.01);
    if (!vc::predict(state, command, 0.01)) {
      return 1;
    }
    if (tick % 10 == 0) {
      std::cout << time + 0.01 << ',' << state.x << ',' << state.y << ',' << state.speed << ','
                << state.yaw << ',' << state.trailer_yaw << ',' << command.acceleration << ','
                << command.steering << ',' << (command.mode == vc::Mode::stopping) << '\n';
    }
  }
  const bool passed = dropout ? state.speed == 0.0 : std::abs(state.speed - 20.0) < 0.05;
  return passed ? 0 : 1;
}
