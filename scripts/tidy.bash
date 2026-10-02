#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
vc_tidy=clang-tidy
if [[ -x .venv/bin/clang-tidy ]]; then vc_tidy=.venv/bin/clang-tidy; fi
# Explicitly match the GCC used by CMake; avoid incomplete, newer GCC installations.
vc_gcc_directory="$(dirname "$(g++ -print-libgcc-file-name)")"
"$vc_tidy" -p "${1:-build/dev}" --extra-arg="--gcc-install-dir=$vc_gcc_directory" \
  src/control.cpp src/demo.cpp tests/core_tests.cpp
