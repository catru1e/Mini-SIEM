#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$PROJECT_ROOT"

if [ ! -f logs/demo_payload.txt ]; then
  echo "[demo] logs/demo_payload.txt not found"
  exit 1
fi

if [ ! -f logs/demo_auth.log ]; then
  touch logs/demo_auth.log
fi

cat logs/demo_payload.txt >> logs/demo_auth.log

echo "[demo] appended demo events to logs/demo_auth.log"
