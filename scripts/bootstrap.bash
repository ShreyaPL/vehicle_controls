#!/usr/bin/env bash
set -eo pipefail
cd "$(dirname "$0")/.."
/usr/bin/python3 -m venv .venv
env -u PYTHONPATH .venv/bin/python -m pip install -r requirements.lock
source scripts/activate.bash
cmake --preset dev
cmake --build --preset dev -j 2
ctest --preset dev
