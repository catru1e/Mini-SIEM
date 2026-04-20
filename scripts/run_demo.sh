#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$PROJECT_ROOT"

mkdir -p logs data

echo "[demo] starting server..."
./scripts/run_server.sh &
SERVER_PID=$!

sleep 2

echo "[demo] starting agent..."
./scripts/run_agent.sh &
AGENT_PID=$!

echo "[demo] server pid: $SERVER_PID"
echo "[demo] agent pid: $AGENT_PID"
echo "[demo] dashboard: http://127.0.0.1:6767"
echo "[demo] press Ctrl+C to stop"

cleanup() {
  echo
  echo "[demo] stopping..."
  kill "$AGENT_PID" 2>/dev/null || true
  kill "$SERVER_PID" 2>/dev/null || true
}

trap cleanup INT TERM

wait
