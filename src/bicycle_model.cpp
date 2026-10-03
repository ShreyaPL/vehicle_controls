#include "vehicle_controls/bicycle_model.hpp"

#include <cmath>

namespace vehicle_controls{
    State step(
        const State& current,
        double speed,
        double steering,
        double wheelbase,
        double dt){
        // Derivates calculated from same current state
        const double x_dot = speed * std::cos(current.yaw);
        const double y_dot = speed * std::sin(current.yaw);
        const double yaw_dot = speed * std::tan(steering) / wheelbase;

        State next;
        next.x = current.x + x_dot * dt;
        next.y = current.y + y_dot * dt;
        next.yaw = current.yaw + yaw_dot * dt;

        // return next state
        return next;
    }
}   // namespace vehicle_controls