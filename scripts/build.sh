#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

echo "Built binaries in: $ROOT_DIR/dist"
ls -la dist
