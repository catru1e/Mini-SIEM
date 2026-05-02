#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$PROJECT_ROOT"

pkill mini_siem_server 2>/dev/null || true
pkill mini_siem_agent 2>/dev/null || true

rm -f data/events.db
rm -f data/agent_state.json
rm -f logs/server.pid
rm -f logs/agent.pid

mkdir -p data logs

: > logs/server.log
: > logs/agent.log
: > logs/demo_auth.log

echo "[reset] demo state cleaned"
echo "[reset] database removed"
echo "[reset] logs cleaned"
echo "[reset] ready to run ./scripts/run_demo.sh"
