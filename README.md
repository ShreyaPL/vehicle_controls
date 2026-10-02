# Articulated Truck Controls Lab

A C++17 learning project for highway tractor–trailer control, vehicle characterization,
software-in-the-loop testing, and safety engineering practice. The starting point is a
small, reviewable control core with explicit limits and fault behavior. This is a
simulation baseline, not road-ready or certified vehicle software.

## Start here

On this machine, the `.venv` environment and dependencies have already been installed.
From this directory:

```bash
source scripts/activate.bash
cmake --preset dev
cmake --build --preset dev -j 2
ctest --preset dev
./build/dev/truck_demo > nominal.csv
./build/dev/truck_demo --dropout > dropout.csv
python scripts/replay.py dropout.csv
```

The last command opens a local MuJoCo viewer. The driving/controller computation is
C++; Python only replays its CSV. A display is needed for the viewer. CSV replay uses
`mj_forward` and is visualization, not an independent vehicle dynamics validation.

For a fresh checkout on Ubuntu 22.04 with ROS 2 Humble, GCC, CMake, Ninja and Python
3.10 with venv support installed, run `bash scripts/bootstrap.bash`. It installs the
pinned Python/native MuJoCo dependencies locally. No global pip installation is needed.
A Python venv isolates tooling and the MuJoCo wheel; native ROS and the compiler remain
system dependencies. Exact Python packages are in `requirements.lock`; system versions
are recorded in [the compatibility report](docs/compatibility.md).

## What runs today

- C++ speed proportional control and curvature feedforward at a nominal 100 Hz.
- Acceleration saturation, steering angle/rate limits, and a speed-dependent steering target.
- Latched stop on invalid input, command age over 100 ms, excessive articulation, or invalid period.
- Shared planar tractor–trailer prediction with forward-only braking to standstill.
- Deterministic 60 s scenarios at 20 m/s: nominal driving and planning dropout after 30 s.
- Native C++ MuJoCo model loading/stepping, plus a geometric articulated scene.
- A ROS 2 C++ simulation adapter for planning commands and odometry.
- CTest regression tests, strict warnings, formatting, static analysis, sanitizer builds, and CI configuration.

The MuJoCo scene has no tire, brake, steering, or suspension dynamics. Driving scenarios
currently use the kinematic predictor as their plant. Independent plant validation is
part of milestone 2, not established by those scenarios.

## ROS 2 simulation

In both terminals run `source scripts/activate.bash`. Use the same isolated test domain:

```bash
export ROS_DOMAIN_ID=87
export ROS_LOCALHOST_ONLY=1
```

Terminal 1:

```bash
./build/dev/truck_ros_sim
```

Terminal 2:

```bash
ros2 topic pub -r 50 /planning/command geometry_msgs/msg/TwistStamped \
  '{twist: {linear: {x: 20.0}, angular: {z: 0.04}}}'
```

Inspect `/sim/odometry` in another terminal. Stopping publication for more than 100 ms
latches braking; restart the simulation node to rearm. `linear.x` is speed in m/s;
`angular.z` is yaw rate in rad/s, converted to curvature by dividing by speed. The node
waits stationary for its first command. It uses monotonic receive time; it does not
validate the sender's timestamp or detect delayed-but-continuously-arriving commands.
This executable is a plain CMake ROS client, not yet a colcon/ament package. It simulates
its own state and has no connection to vehicle hardware. Odometry contains tractor pose
and longitudinal speed only; covariance and full angular/twist estimation are future work.

## Development

```bash
bash scripts/check.bash          # format, build, tests, tidy, ASan/UBSan
cmake --preset release
cmake --build --preset release -j 2
ctest --preset release
.venv/bin/python scripts/system_check.py
/usr/bin/python3 scripts/ros_smoke.py  # local DDS acceleration/dropout/latch check
```

`vc_core` has no ROS, MuJoCo, heap allocation, threads, or I/O dependencies in its update
path. Each controller belongs to one execution context. ROS serialization, logging,
and simulation setup are outside that core; real-time scheduling is not guaranteed.
CI covers the core and MuJoCo fixture; ROS runtime tests are local integration checks.

Read [the roadmap](docs/roadmap.md), [requirements and traceability](docs/requirements.md),
and [architecture and stability notes](docs/architecture.md). Start with the characterization
milestone before tuning more advanced controllers.

## Git workflow

Use a branch per milestone, keep changes small, run `bash scripts/check.bash`, then commit
code, requirements, and validation evidence together. `.venv`, build output, ROS logs,
and scenario CSVs are ignored. No remote repository is configured automatically.
