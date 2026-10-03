# Articulated Truck Controls Lab

Environment and planning scaffold for a C++17 project you will implement yourself.
A MuJoCo visualization scene is included. You will implement the vehicle motion,
controller, ROS nodes, drivers, and tests yourself.

## Test the initial setup

From the project directory:

```bash
bash scripts/check.bash
```

This checks Python dependency consistency, imports MuJoCo, runs the formatting/analysis
executables, and configures CMake to verify the C++ compiler, Ninja, ROS 2 packages,
and MuJoCo native headers/library. Success ends with `Setup check passed`.
It does not validate vehicle behavior or ROS communication and does not run project tests.

To activate the environment for your work:

```bash
source scripts/activate.bash
```

For an inventory of installed versions:

```bash
.venv/bin/python scripts/system_check.py
```

## Environment

The existing `.venv` contains pinned MuJoCo, NumPy, clang-format and clang-tidy packages.
ROS 2 Humble and the compiler are system dependencies. This environment uses Ubuntu
22.04 and Python 3.10. See [compatibility notes](docs/compatibility.md).

On a fresh checkout with those system prerequisites and Python venv support installed:

```bash
bash scripts/bootstrap.bash
```

CMake currently discovers dependencies only. Add your own source files, executable/library
targets, warning policies, and tests when ready. Formatting and analysis configuration is
retained for use with your future code. No application CI workflow is installed yet.

## Project planning and Git

Follow [the roadmap](docs/roadmap.md) at your own pace. The earlier generated implementation
was reverted using Git; it is retained only in history. The current tree contains setup
and planning files. The Python inventory and shell scripts are environment tooling only.
Use a branch per milestone and commit your own implementation and validation evidence.

## MuJoCo lane and vehicle

```bash
source scripts/activate.bash
python -m mujoco.viewer --mjcf=models/lane_turn.xml
```

The car remains stationary until you supply its pose. This is a visualization scene,
not an actuated vehicle physics model. See [scene notes](models/README.md) for geometry
and the pose interface. Open lane_turn.xml, not the vehicle.xml include fragment.
