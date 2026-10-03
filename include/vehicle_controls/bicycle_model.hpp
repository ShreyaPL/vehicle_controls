#pragma once

namespace vehicle_controls{
// Position of the rear axle and vehicle heading.
struct State{
    double x{0.0};
    double y{0.0};
    double yaw{0.0};
};

// Return the state after one timestep
// Assume finite inputs, positive wheelbase and timestep
// and steering angle strictly between -pi/2 and pi/2.
[[nodiscard]] State step(
    const State& current, 
    double speed,
    double steering,
    double wheelbase,
    double dt);
}   // namespace vehicle_controls