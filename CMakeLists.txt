cmake_minimum_required(VERSION 3.22)
project(vehicle_controls VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependency discovery only. Add your own source files and targets when ready.
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
set(VC_PYTHON "${CMAKE_SOURCE_DIR}/.venv/bin/python" CACHE FILEPATH "Python with MuJoCo installed")
execute_process(COMMAND "${VC_PYTHON}" -c "import pathlib,mujoco; print(pathlib.Path(mujoco.__file__).parent)"
  OUTPUT_VARIABLE MUJOCO_ROOT OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
find_path(MUJOCO_INCLUDE mujoco/mujoco.h PATHS "${MUJOCO_ROOT}/include" NO_DEFAULT_PATH REQUIRED)
find_library(MUJOCO_LIBRARY NAMES mujoco libmujoco.so.3.3.7 PATHS "${MUJOCO_ROOT}" NO_DEFAULT_PATH REQUIRED)
message(STATUS "Setup dependencies found. No implementation or test targets have been added.")
