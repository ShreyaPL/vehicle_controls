#!/usr/bin/env bash
set -eo pipefail
cd "$(dirname "$0")/.."
/usr/bin/python3 -m venv .venv
env -u PYTHONPATH .venv/bin/python -m pip install -r requirements.lock
bash scripts/check.bash
