#!/usr/bin/env bash
set -eo pipefail
cd "$(dirname "$0")/.."
source scripts/activate.bash
env -u PYTHONPATH .venv/bin/python -m pip check
.venv/bin/python -c 'import mujoco; print("MuJoCo:", mujoco.__version__)'
.venv/bin/clang-format --version
.venv/bin/clang-tidy --version
cmake --preset dev
printf '%s\n' 'Setup check passed: dependencies found; no project implementation or tests yet.'
