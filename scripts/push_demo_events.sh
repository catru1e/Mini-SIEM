#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$PROJECT_ROOT"

PAYLOAD_FILE="${1:-logs/demo_payload.txt}"
TARGET_LOG="logs/demo_auth.log"

if [ ! -f "$PAYLOAD_FILE" ]; then
  echo "[demo] payload file not found: $PAYLOAD_FILE"
  exit 1
fi

mkdir -p logs
touch "$TARGET_LOG"

cat "$PAYLOAD_FILE" >> "$TARGET_LOG"

echo "[demo] appended payload from: $PAYLOAD_FILE"
echo "[demo] target log: $TARGET_LOG"
