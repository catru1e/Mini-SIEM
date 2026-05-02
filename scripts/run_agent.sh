#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$PROJECT_ROOT"

mkdir -p logs data

echo "[run_agent] project root: $PROJECT_ROOT"
echo "[run_agent] starting agent..."

./dist/mini_siem_agent
