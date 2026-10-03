#include "vehicle_controls/bicycle_model.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

int main(){
    constexpr double speed = 3.0;
    constexpr double wheelbase = 2.8;
    constexpr double dt = 0.01;
    constexpr double steps = 500;

    std::cout << std::fixed << std::setprecision(4);

    // Experiment 1: Straight Driving
    vehicle_controls::State straight{5.0, 0.0, 0.0};

    for (int i = 0; i < steps; i++){
        straight = vehicle_controls::step(
            straight, speed, 0.0, wheelbase, dt);
    }

    std::cout << "Straight driving after 5 seconds:\n"
              << " x:   " << straight.x << " m\n"
              << " y:   " << straight.y << " m\n"
              << " yaw: " << straight.yaw << " rad\n";
    
    // Experiment 2: start at beginning of road turn.
    constexpr double radius = 30.0;
    const double steering = std::atan(wheelbase / radius);

    vehicle_controls::State turning{30.0, 0.0, 0.0};

    for(int i = 0; i < steps; i++){
        turning = vehicle_controls::step(
            turning, speed, steering, wheelbase, dt);
    }

    // Analytical circular-motion solution for comparison
    const double duration = steps * dt;
    const double expected_yaw = speed * duration / radius;
    const double expected_x = 30.0 + radius * std::sin(expected_yaw);
    const double expected_y = radius * (1.0 - std::cos(expected_yaw));

    std::cout << "\nLeft turn after 5 seconds:\n"
            << "  Steering: " << steering << " rad\n"
            << "  Computed: x=" << turning.x
            << ", y=" << turning.y
            << ", yaw=" << turning.yaw << '\n'
            << "  Expected: x=" << expected_x
            << ", y=" << expected_y
            << ", yaw=" << expected_yaw << '\n';
    
    return 0;
}