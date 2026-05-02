#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$PROJECT_ROOT"

mkdir -p logs data

SERVER_LOG="logs/server.log"
AGENT_LOG="logs/agent.log"
SERVER_PID_FILE="logs/server.pid"
AGENT_PID_FILE="logs/agent.pid"

PORT="$(python3 -c 'import json; print(json.load(open("config/config.json")).get("server", {}).get("port", 6767))' 2>/dev/null || echo 6767)"

GREEN="\033[0;32m"
YELLOW="\033[1;33m"
RED="\033[0;31m"
BLUE="\033[0;34m"
RESET="\033[0m"

cleanup() {
  echo
  echo -e "${YELLOW}[demo] stopping Mini-SIEM...${RESET}"

  if [ -f "$AGENT_PID_FILE" ]; then
    kill "$(cat "$AGENT_PID_FILE")" 2>/dev/null || true
    rm -f "$AGENT_PID_FILE"
  fi

  if [ -f "$SERVER_PID_FILE" ]; then
    kill "$(cat "$SERVER_PID_FILE")" 2>/dev/null || true
    rm -f "$SERVER_PID_FILE"
  fi

  echo -e "${GREEN}[demo] stopped.${RESET}"
}

status_line() {
  local name="$1"
  local pid_file="$2"

  if [ -f "$pid_file" ] && kill -0 "$(cat "$pid_file")" 2>/dev/null; then
    echo -e "${GREEN}[OK]${RESET} $name running, pid=$(cat "$pid_file")"
  else
    echo -e "${RED}[--]${RESET} $name stopped"
  fi
}

wait_for_server() {
  echo -e "${YELLOW}[demo] waiting for server health...${RESET}"

  for i in {1..30}; do
    if curl -fsS "http://127.0.0.1:${PORT}/health" >/dev/null 2>&1; then
      echo -e "${GREEN}[demo] server is healthy.${RESET}"
      return 0
    fi
    sleep 0.3
  done

  echo -e "${RED}[demo] server health check failed.${RESET}"
  echo "[demo] last server log lines:"
  tail -n 20 "$SERVER_LOG" || true
  return 1
}

start_server() {
  echo -e "${BLUE}[demo] starting server...${RESET}"
  : > "$SERVER_LOG"
  ./dist/mini_siem_server >> "$SERVER_LOG" 2>&1 &
  echo $! > "$SERVER_PID_FILE"
}

start_agent() {
  echo -e "${BLUE}[demo] starting agent...${RESET}"
  : > "$AGENT_LOG"
  ./dist/mini_siem_agent >> "$AGENT_LOG" 2>&1 &
  echo $! > "$AGENT_PID_FILE"
}

view_log() {
  local file="$1"
  local title="$2"

  if [ ! -f "$file" ]; then
    echo "[viewer] log file not found: $file"
    echo "Press Enter to continue..."
    read -r
    return
  fi

  local total_lines
  total_lines=$(wc -l < "$file")

  local height
  height=$(tput lines 2>/dev/null || echo 24)

  local page_size=$((height - 8))
  if [ "$page_size" -lt 5 ]; then
    page_size=5
  fi

  local top=1
  local key=""

  while true; do
    total_lines=$(wc -l < "$file")

    if [ "$total_lines" -lt 1 ]; then
      total_lines=1
    fi

    local max_top=$((total_lines - page_size + 1))
    if [ "$max_top" -lt 1 ]; then
      max_top=1
    fi

    if [ "$top" -lt 1 ]; then
      top=1
    fi

    if [ "$top" -gt "$max_top" ]; then
      top="$max_top"
    fi

    local bottom=$((top + page_size - 1))
    if [ "$bottom" -gt "$total_lines" ]; then
      bottom="$total_lines"
    fi

    clear
    echo "============================================"
    echo "  $title"
    echo "============================================"
    echo "File: $file"
    echo "Lines: $top-$bottom / $total_lines"
    echo
    echo "Controls: w=up, s=down, a=page up, d=page down, g=start, G=end, q=back"
    echo "--------------------------------------------"

    sed -n "${top},${bottom}p" "$file"

    read -rsn1 key

    case "$key" in
      w)
        top=$((top - 1))
        ;;
      s)
        top=$((top + 1))
        ;;
      a)
        top=$((top - page_size))
        ;;
      d)
        top=$((top + page_size))
        ;;
      g)
        top=1
        ;;
      G)
        top="$max_top"
        ;;
      q)
        break
        ;;
    esac
  done
}

draw_screen() {
  clear
  echo -e "${BLUE}============================================${RESET}"
  echo -e "${BLUE}          MINI-SIEM USB DEMO LAUNCHER        ${RESET}"
  echo -e "${BLUE}============================================${RESET}"
  echo
  echo "Project root: $PROJECT_ROOT"
  echo "Dashboard:    http://127.0.0.1:${PORT}"
  echo
  status_line "Server" "$SERVER_PID_FILE"
  status_line "Agent " "$AGENT_PID_FILE"
  echo
  echo -e "${YELLOW}Keys:${RESET}"
  echo "  d  inject demo events from selected file"
  echo "  s  open full server log"
  echo "  a  open full agent log"
  echo "  q  stop server, stop agent, and quit"
  echo
  echo -e "${YELLOW}Log viewer help:${RESET}"
  echo "  Press s or a to open a full log viewer."
  echo "  Inside log viewer:"
  echo "    w  scroll up one line"
  echo "    s  scroll down one line"
  echo "    a  scroll up one page"
  echo "    d  scroll down one page"
  echo "    g  go to the beginning"
  echo "    G  go to the end"
  echo "    q  return to launcher"
  echo
}

trap cleanup INT TERM EXIT

start_server
wait_for_server
start_agent

while true; do
  draw_screen
  read -rsn1 key

  case "$key" in
    d)
      echo
      echo -e "${BLUE}[demo] injecting demo events...${RESET}"
      echo "Enter payload file path."
      echo "Example: logs/demo_payload.txt"
      echo "Example: logs/bruteforce.txt"
      echo "Example: /home/ctrl/Desktop/test_logs.txt"
      echo
      read -rp "Payload file [logs/demo_payload.txt]: " payload_file

      if [ -z "$payload_file" ]; then
        payload_file="logs/demo_payload.txt"
      fi

      if ./scripts/push_demo_events.sh "$payload_file"; then
        echo -e "${GREEN}[demo] demo events injected.${RESET}"
      else
        echo -e "${RED}[demo] failed to inject demo events.${RESET}"
        echo "[demo] check file path: $payload_file"
      fi

        echo
        echo "Press Enter to continue..."
      read -r
      ;;
    s)
      view_log "$SERVER_LOG" "FULL SERVER LOG"
      ;;
    a)
      view_log "$AGENT_LOG" "FULL AGENT LOG"
      ;;
    q)
      exit 0
      ;;
  esac
done
