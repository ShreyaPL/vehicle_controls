#pragma once
#include <cstdint>

namespace vc {
// SI units; planar tractor rear axle state, on-axle hitch, forward motion only.
struct State {
  double x{0.0};
  double y{0.0};
  double yaw{0.0};
  double trailer_yaw{0.0};
  double speed{0.0};
};
struct Reference {
  double speed{0.0};
  double curvature{0.0};
  double age{0.0}; // Seconds since receipt, measured by a monotonic clock.
};
enum class Mode : std::uint8_t { nominal, stopping };
struct Actuation {
  double acceleration{0.0};
  double steering{0.0};
  Mode mode{Mode::nominal};
};
class Controller {
public:
  // Fixed-work, allocation-free, single-owner API; fault stop latches until restart.
  [[nodiscard]] Actuation update(const State &state, const Reference &reference,
                                 double dt) noexcept;

private:
  Mode mode_{Mode::nominal};
  double steering_{0.0};
};
// Returns false without changing state for invalid input. Not a tire dynamics model.
[[nodiscard]] bool predict(State &state, const Actuation &command, double dt) noexcept;
[[nodiscard]] double angle_difference(double a, double b) noexcept;
} // namespace vc
