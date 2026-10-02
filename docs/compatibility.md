# Compatibility and validation report

Checked 2026-10-01 on this workstation. Machine-readable details: [system_inventory.json](system_inventory.json).

| Component | Observed version/status | Result |
|---|---|---|
| OS / architecture | Ubuntu 22.04.5, x86_64 | Suitable for the tested baseline |
| Kernel | 6.8.0-138-generic | General-purpose Linux; no hard real-time qualification |
| ROS 2 | Humble, rclcpp 16.0.21 | C++ compile and local DDS integration passed |
| GCC | 11.4.0 | C++17 debug/release and sanitizers passed |
| CMake | 3.22.1 | Presets and native dependency discovery passed |
| Ninja / Git / colcon | Available | Ninja used; colcon not required for this plain CMake project |
| Python | System 3.10.12, project `.venv` | Environment created successfully |
| MuJoCo | Project-local 3.3.7 | Native C++ header/runtime match and scene stepping passed |
| NumPy | Project-local 1.26.4 | Dependency check passed |
| clang-format / clang-tidy | Project-local 18.1.8 | Formatting and core static analysis passed |
| OpenGL | GL and GLFW system libraries present | MuJoCo EGL offscreen render passed (160×120 RGB) |

ROS Humble was released for Ubuntu 22.04, with announced support until May 2027
([Open Robotics release announcement](https://www.openrobotics.org/blog/2022/5/24/ros-2-humble-hawksbill-release)).
Plan an OS/ROS migration separately when needed; this setup retains the installed system.
The MuJoCo Python package includes the native library
([official installation documentation](https://mujoco.readthedocs.io/en/stable/python.html#installation));
CMake links that library directly from C++ without a Python control loop.

MuJoCo was not importable from `/usr/bin/python3` at initial inspection. Another user
installation may exist in a different environment; it was not modified. The project now
has its own version. All Python runtime/tool dependencies are pinned in requirements.lock.
System ROS, compiler, kernel and graphics drivers are recorded rather than isolated by venv.

## Executed checks

- Debug build: all 11 CTest tests passed, including native MuJoCo fixture.
- Release build: all 11 CTest tests passed; checks are not compiled out by NDEBUG.
- Address/undefined-behavior sanitizers: all 10 core/scenario tests passed with leak detection enabled.
- clang-format: passed. clang-tidy: passed for control.cpp, demo.cpp, core_tests.cpp;
  diagnostics in third-party/system headers are suppressed by the configured scope.
- ROS runtime: `scripts/ros_smoke.py` observed 504 odometry messages and verified acceleration,
  command-loss braking to standstill, and no automatic restart after commands resumed.
- MuJoCo EGL: a 160×120 RGB image rendered with nonconstant pixel values.
- Python dependency consistency: `env -u PYTHONPATH .venv/bin/python -m pip check` passed.

The interactive desktop viewer has not been launched. CI configuration is supplied but
has not run on a remote service. CAN, an RTOS target, hardware timing, real truck dynamics,
and certification/compliance have not been verified or implemented.

## Environment details resolved

ROS adds Python packages via PYTHONPATH. Dependency locking and pip checks explicitly
remove that variable so system ROS packages are not treated as pip-managed dependencies.
ROS commands continue to use the installed system Python; tooling uses `.venv`.

clang-tidy needed an explicit GCC installation path on this host to locate the correct
standard-library headers. scripts/tidy.bash derives that path from the installed g++.

The execution sandbox blocks DDS sockets and LeakSanitizer process inspection. The ROS
integration and sanitizer tests passed outside that sandbox with permission; the tests
were not weakened to bypass those restrictions. On a normal local terminal:

```bash
source scripts/activate.bash
bash scripts/check.bash
/usr/bin/python3 scripts/ros_smoke.py
```
