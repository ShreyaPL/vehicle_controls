#!/usr/bin/env bash
set -eo pipefail
cd "$(dirname "$0")/.."
source scripts/activate.bash
env -u PYTHONPATH .venv/bin/python -m pip check
.venv/bin/clang-format --dry-run --Werror include/vehicle_controls/*.hpp src/*.cpp tests/*.cpp
cmake --preset dev
cmake --build --preset dev -j 2
ctest --preset dev
bash scripts/tidy.bash build/dev
cmake --preset asan
cmake --build --preset asan -j 2
ctest --preset asan
