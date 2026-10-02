#include "vehicle_controls/control.hpp"
#include <chrono>
#include <cmath>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <memory>
#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>

// Software-in-the-loop only. Planning command: linear.x speed (m/s), angular.z yaw rate (rad/s).
class TruckSimulation final : public rclcpp::Node {
public:
  TruckSimulation() : Node("truck_simulation") {
    publisher_ = create_publisher<nav_msgs::msg::Odometry>("sim/odometry", 10);
    subscription_ = create_subscription<geometry_msgs::msg::TwistStamped>(
        "planning/command", rclcpp::QoS(1).best_effort(),
        [this](geometry_msgs::msg::TwistStamped::ConstSharedPtr msg) {
          reference_.speed = msg->twist.linear.x;
          reference_.curvature =
              std::abs(reference_.speed) > 0.01 ? msg->twist.angular.z / reference_.speed : 0.0;
          if (!std::isfinite(msg->twist.angular.z) ||
              (std::abs(reference_.speed) <= 0.01 && msg->twist.angular.z != 0.0)) {
            reference_.age = -1.0;
            rejected_ = true;
          }
          received_ = Clock::now();
          have_command_ = true;
        });
    last_tick_ = Clock::now();
    timer_ = create_wall_timer(std::chrono::milliseconds(10), [this] { tick(); });
  }

private:
  using Clock = std::chrono::steady_clock;
  void tick() {
    const auto time = Clock::now();
    const double dt = std::chrono::duration<double>(time - last_tick_).count();
    last_tick_ = time;
    // Wait stationary for first command. Once running, timeout is latched by the core.
    if (!have_command_) {
      return;
    }
    reference_.age = rejected_ ? -1.0 : std::chrono::duration<double>(time - received_).count();
    const auto command = controller_.update(state_, reference_, dt);
    if (!vc::predict(state_, command, dt)) {
      RCLCPP_ERROR(get_logger(), "Simulation timing/state invalid; terminating simulation");
      rclcpp::shutdown();
      return;
    }
    nav_msgs::msg::Odometry output;
    output.header.stamp = now();
    output.header.frame_id = "map";
    output.child_frame_id = "tractor";
    output.pose.pose.position.x = state_.x;
    output.pose.pose.position.y = state_.y;
    output.pose.pose.orientation.z = std::sin(state_.yaw / 2.0);
    output.pose.pose.orientation.w = std::cos(state_.yaw / 2.0);
    output.twist.twist.linear.x = state_.speed;
    publisher_->publish(output);
  }
  vc::Controller controller_;
  vc::State state_;
  vc::Reference reference_;
  bool have_command_{false};
  bool rejected_{false};
  Clock::time_point received_{};
  Clock::time_point last_tick_{};
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr publisher_;
  rclcpp::Subscription<geometry_msgs::msg::TwistStamped>::SharedPtr subscription_;
  rclcpp::TimerBase::SharedPtr timer_;
};
int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TruckSimulation>());
  rclcpp::shutdown();
}
