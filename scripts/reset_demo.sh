#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$PROJECT_ROOT"

RESET_STATE=0

if [ "${1:-}" = "--with-state" ]; then
  RESET_STATE=1
fi

pkill mini_siem_server 2>/dev/null || true
sudo pkill mini_siem_agent 2>/dev/null || true

rm -f logs/server.pid
rm -f logs/agent.pid
rm -f data/events.db

mkdir -p data logs exports

: > logs/server.log
: > logs/agent.log
: > logs/demo_auth.log
touch logs/demo_auth.log

if [ "$RESET_STATE" -eq 1 ]; then
  rm -f data/agent_state.json
  echo "[reset] agent state removed"
else
  echo "[reset] agent state preserved"
  echo "[reset] use ./scripts/reset_demo.sh --with-state only if you really want to reread logs from the beginning"
fi

sudo chown -R "$(id -u):$(id -g)" logs data exports 2>/dev/null || true

echo "[reset] database removed"
echo "[reset] logs cleaned"
echo "[reset] ready to run ./scripts/run_demo.sh"
