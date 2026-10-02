# Baseline requirements and traceability

These are learning-project requirements, not an ISO 26262 safety case. Numbers are
initial engineering assumptions, to be revisited after characterization. Tests use
explicit checks that remain enabled in release builds.

| ID | Requirement | Implementation | Verification |
|---|---|---|---|
| CTRL-001 | Valid forward commands produce acceleration in [-3, 1.5] m/s² | Controller::update | core_limits |
| CTRL-002 | Nominal steering remains within ±0.45 rad and changes at <=0.3 rad/s | Controller::update | core_limits |
| CTRL-003 | With ideal plant and target 20 m/s, speed error is <0.05 m/s at 60 s | Controller::update | core_nominal, scenario_nominal |
| SAFE-001 | Command receive age >0.1 s requests -3 m/s² within the next update | Controller::update | core_stale, scenario_dropout |
| SAFE-002 | Non-finite inputs or dt outside (0,0.1] latch stopping with finite output | Controller::update | core_invalid |
| SAFE-003 | Absolute wrapped articulation >0.6 rad latches stopping | Controller::update | core_articulation |
| SAFE-004 | A latched stop cannot recover merely because commands resume | Controller::update | core_latch |
| MODEL-001 | Straight constant-speed prediction matches analytical distance | predict | core_prediction |
| MODEL-002 | Invalid actuation is rejected without modifying state | predict | core_prediction |
| MODEL-003 | Straight braking at -3 m/s² from 20 m/s stops at v²/(2a), without reverse motion | predict | core_braking |
| BUILD-001 | Native MuJoCo headers/library match and scene steps 2 s without warnings | mujoco_check | mujoco_native |

Tests provide example-based evidence, not universal proofs. Core model limits and the
ROS receive-time contract are documented in architecture.md. State freshness, source
sequence, steering/braking health, lateral path containment and sensor disagreement
are future requirements, not covered by the initial controller.

## Initial hazard worksheet

| Hazard | Initial simulated response | Remaining work |
|---|---|---|
| Planning stream lost | Latched deceleration | Validate delay budget and independent watchdog |
| Invalid numeric input | Finite stop command | Independent state plausibility and sensor redundancy |
| Large articulation | Latched deceleration | Identify recoverable envelope; verify loaded dynamics |
| Steering stuck | Not implemented | Feedback tracking, fault injection, path feasibility |
| Braking unavailable | Not implemented | Redundant braking architecture and fault containment |
| Execution deadline missed | Core stop; ROS simulation exits on invalid interval | Target timing measurements, external watchdog |

## Review process

For each change: write or revise a requirement, implement it, add independent evidence,
record assumptions and limitations, and review the resulting diff. Link requirements to
named tests rather than relying on coverage percentage alone. Keep failed scenarios as
regression cases. Introduce change impact analysis and fault-tree/FMEA reviews as models grow.
