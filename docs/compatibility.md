# Setup compatibility

Previously inspected on 2026-10-01: Ubuntu 22.04.5 x86_64, ROS 2 Humble,
GCC 11.4.0, CMake 3.22.1, Ninja, Git, colcon, and Python 3.10.12.
The original machine inventory is retained in [system_inventory.json](system_inventory.json).

Project-local dependencies: MuJoCo 3.3.7, NumPy 1.26.4, clang-format 18.1.8,
and clang-tidy 18.1.8, with transitive dependencies pinned in requirements.lock.
The venv isolates these packages, not system ROS, the compiler, kernel, or graphics drivers.

Run `bash scripts/check.bash` to check the current setup. It checks dependency consistency,
MuJoCo import, tool executables, compiler configuration, ROS development-package discovery,
and native MuJoCo header/library discovery. No project code is compiled or tested yet.

ROS adds system Python packages through PYTHONPATH. Pip operations remove this variable
so the environment lock does not include system ROS packages. Environment activation
retains ROS's normal paths for ROS tools.

Prior implementation tests are no longer part of this project and do not establish
anything about the code you will write. Interactive rendering, DDS communication, CAN,
RTOS execution, and real-time behavior are outside the setup-only check.
