# Project milestones

The repository starts at milestone 1. Implement the next milestones on separate Git
branches, with requirements and measured acceptance results in each change.

| Milestone | Deliverable | Completion evidence |
|---|---|---|
| 1. Reproducible baseline | C++ core, venv, ROS build, model fixture, unit/scenario tests | Local build/test report and first commit |
| 2. Platform characterization | MuJoCo actuated tractor/trailer plant; acceleration/brake steps, steering sweeps, payload/grade cases | Independent experiment data; fit gain, delay and time constants; held-out prediction error |
| 3. Vehicle prediction | Dynamic bicycle and trailer model, tire cornering stiffness, actuator lags | Compare against independent plant/data at multiple speeds; discretization sensitivity |
| 4. Tracking control | PID speed control with anti-windup; lateral LQR or MPC; trajectory interface | Lane tracking and speed error distributions; saturation handling; local stability analysis |
| 5. Degraded operation | Fault injection for sensor stale data, dropped commands, stuck steering, reduced brakes | Defined operating envelope, stop distance, path containment and fault detection latency |
| 6. Low-level interface | Documented simulated CAN protocol, encoder/decoder tests, SocketCAN vcan transport | Golden frame vectors, byte order/scaling/CRC/counter checks, bus timeouts; no real CAN needed |
| 7. Embedded execution | Port dependency-free core to Zephyr or FreeRTOS target, then hardware-in-loop | Memory budget, stack high-water mark, measured timing/jitter and watchdog tests |
| 8. Assurance portfolio | Versioned requirements, FMEA, traceability, review records and formal properties | Model-check fault-state transitions; bounded C++ checks where supported; documented tool assumptions |

Suggested first experiment: create a longitudinal MuJoCo actuator model with a first-order
lag and saturation. Apply several step commands at two payloads. Fit the lag and gain
from one dataset and evaluate prediction error on a different command sequence. Keep
plant parameters separate from controller parameters so validation cannot silently use
the same model on both sides.

For the portfolio, provide a short demonstration video, architecture diagram, reproducible
build, measured control results, a documented fault-injection example, and a requirement-
to-test matrix. Explain where the model is inaccurate and which claims remain unproven.

Future tool choices (CAN utilities, RTOS SDK, solvers, formal verification tools) should
be pinned when those milestones start. They are not dependencies of this baseline and
have not been installed system-wide.
