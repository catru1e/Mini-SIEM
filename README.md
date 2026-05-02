# Mini-SIEM USB Run Guide

This guide explains how to run the Mini-SIEM project from a USB drive on Linux.

The project is designed as a portable directory:

```text
mini-siem-usb/
├── agent/
├── server/
├── common/
├── config/
├── data/
├── dist/
├── logs/
├── scripts/
├── web/
├── CMakeLists.txt
└── RUN.md
```

---

## 1. Mount USB with exec permission

If the USB drive is mounted with `noexec`, Linux will not allow running binaries or scripts directly from the USB drive.

Typical error:

```bash
Permission denied
```

Example:

```bash
./dist/mini_siem_server
bash: ./dist/mini_siem_server: Permission denied
```

To fix this, remount the USB drive with the `exec` option.

First check where the USB drive is mounted:

```bash
findmnt
```

If the USB is mounted at:

```text
/media/ctrl/MINI-SIEM
```

then run:

```bash
cd /

DEV=$(findmnt -no SOURCE /media/ctrl/MINI-SIEM)

sudo umount /media/ctrl/MINI-SIEM

sudo mkdir -p /mnt/mini-siem-usb

sudo mount -o uid=$(id -u),gid=$(id -g),fmask=0022,dmask=0022,exec "$DEV" /mnt/mini-siem-usb
```

After that, go to the project directory:

```bash
cd /mnt/mini-siem-usb/mini-siem-usb
```

Check that scripts and binaries are executable:

```bash
ls -la scripts
ls -la dist
```

Expected result:

```text
-rwxr-xr-x build.sh
-rwxr-xr-x run_server.sh
-rwxr-xr-x run_agent.sh
-rwxr-xr-x run_demo.sh
-rwxr-xr-x push_demo_events.sh

-rwxr-xr-x mini_siem_server
-rwxr-xr-x mini_siem_agent
```

---

## 2. Build the project

From the project root:

```bash
cd /mnt/mini-siem-usb/mini-siem-usb
```

Run:

```bash
./scripts/build.sh
```

If the script cannot be started directly, run it through bash:

```bash
bash scripts/build.sh
```

Expected result:

```text
Built binaries in: /mnt/mini-siem-usb/mini-siem-usb/dist
```

The following binaries should exist:

```text
dist/mini_siem_server
dist/mini_siem_agent
```

---

## 3. Reset demo state

Before a clean demonstration, reset old runtime data:

```bash
./scripts/reset_demo.sh
```

This script stops old Mini-SIEM processes if they are running and cleans:

```text
data/events.db
data/agent_state.json
logs/server.log
logs/agent.log
logs/demo_auth.log
```

Expected result:

```text
[reset] demo state cleaned
[reset] database removed
[reset] logs cleaned
[reset] ready to run ./scripts/run_demo.sh
```

---

## 4. Start the full demo launcher

Run:

```bash
./scripts/run_demo.sh
```

The launcher starts:

```text
mini_siem_server
mini_siem_agent
```

Expected screen:

```text
============================================
          MINI-SIEM USB DEMO LAUNCHER
============================================

Project root: /mnt/mini-siem-usb/mini-siem-usb
Dashboard:    http://127.0.0.1:6767

[OK] Server running
[OK] Agent  running

Keys:
  d  inject demo events from selected file
  s  open full server log
  a  open full agent log
  q  stop server, stop agent, and quit
```

---

## 5. Open the dashboard

Open in browser:

```text
http://127.0.0.1:6767
```

The dashboard should show Mini-SIEM events and alerts.

---

## 6. Launcher controls

Inside `run_demo.sh`:

```text
d  inject demo events from selected file
s  open full server log
a  open full agent log
q  stop server, stop agent, and quit
```

---

## 7. Log viewer controls

When opening logs from the launcher, use WASD-style navigation:

```text
w  scroll up one line
s  scroll down one line
a  scroll up one page
d  scroll down one page
g  go to the beginning
G  go to the end
q  return to launcher
```

---

## 8. Inject demo events

Press:

```text
d
```

The launcher will ask for a payload file.

Example:

```text
Payload file [logs/demo_payload.txt]:
```

You can enter one of the prepared files:

```text
logs/bruteforce.txt
logs/sudo_test.txt
logs/full_attack.txt
```

Or press Enter to use the default:

```text
logs/demo_payload.txt
```

The selected payload file is appended to:

```text
logs/demo_auth.log
```

The agent reads `logs/demo_auth.log`, parses the log lines, sends normalized events to the server, and the dashboard displays them.

Pipeline:

```text
payload file
    ↓
scripts/push_demo_events.sh
    ↓
logs/demo_auth.log
    ↓
mini_siem_agent
    ↓
mini_siem_server
    ↓
SQLite database
    ↓
dashboard
```

---

## 9. Example demo payload: brute force

Create or edit:

```bash
nano logs/bruteforce.txt
```

Example content:

```text
Apr 20 22:30:00 smth sshd[12345]: Failed password for root from 1.2.3.4 port 5555 ssh2
Apr 20 22:30:01 smth sshd[12346]: Failed password for root from 1.2.3.4 port 5556 ssh2
Apr 20 22:30:02 smth sshd[12347]: Failed password for root from 1.2.3.4 port 5557 ssh2
Apr 20 22:30:03 smth sshd[12348]: Failed password for root from 1.2.3.4 port 5558 ssh2
Apr 20 22:30:04 smth sshd[12349]: Failed password for root from 1.2.3.4 port 5559 ssh2
Apr 20 22:30:05 smth sshd[12350]: Accepted password for root from 1.2.3.4 port 5560 ssh2
```

This scenario demonstrates:

```text
auth_failed events
auth_success event
brute-force detection
possible correlation alert
```

---

## 10. Example demo payload: sudo activity

Create or edit:

```bash
nano logs/sudo_test.txt
```

Example content:

```text
Apr 20 22:31:00 smth sudo:     ctrl : TTY=pts/0 ; PWD=/home/ctrl ; USER=root ; COMMAND=/usr/bin/nano config/config.json
Apr 20 22:31:01 smth sudo:     ctrl : TTY=pts/0 ; PWD=/home/ctrl ; USER=root ; COMMAND=/usr/bin/ls /root
Apr 20 22:31:02 smth sudo:     ctrl : TTY=pts/0 ; PWD=/home/ctrl ; USER=root ; COMMAND=/usr/bin/cat /etc/shadow
```

This scenario demonstrates:

```text
privilege_escalation events
sudo command parsing
suspicious sudo usage detection
```

---

## 11. Manual server start

If needed, the server can be started manually:

```bash
./scripts/run_server.sh
```

Or directly:

```bash
./dist/mini_siem_server
```

Expected output:

```text
[mini-siem-server] listening on http://127.0.0.1:6767
```

Check health:

```bash
curl http://127.0.0.1:6767/health
```

Expected result:

```text
OK
```

---

## 12. Manual agent start

In another terminal:

```bash
./scripts/run_agent.sh
```

Or directly:

```bash
./dist/mini_siem_agent
```

Expected output:

```text
[mini-siem-agent] started
```

The agent reads log files configured in:

```text
config/config.json
```

For demo mode, the first log source should be:

```text
logs/demo_auth.log
```

Example config:

```json
"agent": {
  "log_paths": [
    "logs/demo_auth.log",
    "/var/log/auth.log"
  ],
  "state_path": "data/agent_state.json",
  "agent_log_path": "logs/agent.log",
  "ssh_watch_path": "~/.ssh"
}
```

---

## 13. API checks

Health:

```bash
curl http://127.0.0.1:6767/health
```

Events:

```bash
curl http://127.0.0.1:6767/api/events
```

Alerts:

```bash
curl http://127.0.0.1:6767/api/alerts
```

---

## 14. When code changes are made

If C++ code was changed, rebuild the project:

```bash
./scripts/build.sh
```

Then restart the demo:

```bash
./scripts/run_demo.sh
```

Examples of files that require rebuild:

```text
server/src/main.cpp
server/src/detect.cpp
server/src/correlate.cpp
server/src/db_sqlite.cpp
agent/src/main.cpp
agent/src/auth_parser.cpp
agent/src/log_reader.cpp
agent/src/process_snapshot.cpp
agent/src/file_monitor.cpp
common/src/config.cpp
```

---

## 15. When rebuild is not required

No rebuild is needed if only these files were changed:

```text
web/index.html
web/app.js
web/style.css
config/config.json
scripts/*.sh
logs/*.txt
```

For web changes, refresh the browser.

For config changes, restart server and agent.

For script changes, run the script again.

---

## 16. Troubleshooting

### Permission denied

If this happens:

```bash
./dist/mini_siem_server
Permission denied
```

The USB is probably mounted with `noexec`.

Fix: remount it with the `exec` option.

---

### CMake cache path error

If this happens:

```text
CMake Error: The current CMakeCache.txt directory is different than the directory where CMakeCache.txt was created
```

It means the project path changed, for example from:

```text
/media/ctrl/MINI-SIEM/mini-siem-usb
```

to:

```text
/mnt/mini-siem-usb/mini-siem-usb
```

Fix:

```bash
rm -rf build
./scripts/build.sh
```

---

### Dashboard does not show new demo events

Check that the agent reads the demo log:

```bash
grep "started, reading log" logs/agent.log
```

Check that demo lines were appended:

```bash
tail -n 20 logs/demo_auth.log
```

Check events directly through API:

```bash
curl http://127.0.0.1:6767/api/events
```

If needed, reset demo state:

```bash
./scripts/reset_demo.sh
./scripts/run_demo.sh
```

---

### Too many process_start events

The agent may send process snapshot events from `/proc`.

This is normal behavior if process monitoring is enabled.

These events look like:

```text
process_start
```

They show that the agent can monitor running processes.

For a cleaner demo, reset the database before starting:

```bash
./scripts/reset_demo.sh
```

---

## 17. Recommended final demo flow

Use this sequence during presentation:

```bash
cd /mnt/mini-siem-usb/mini-siem-usb
./scripts/reset_demo.sh
./scripts/build.sh
./scripts/run_demo.sh
```

Then:

```text
1. Open http://127.0.0.1:6767
2. Press d in the launcher
3. Choose logs/bruteforce.txt
4. Show events in dashboard
5. Show generated alerts
6. Press d again
7. Choose logs/sudo_test.txt
8. Show privilege escalation events
9. Press s to show full server log
10. Press a to show full agent log
11. Press q to stop everything
```

---

## 18. What this demonstrates

The project demonstrates a simplified SIEM system:

```text
log collection
event normalization
SQLite storage
detection rules
alert generation
event correlation
process monitoring
file monitoring
web dashboard
portable USB execution
```

Main components:

```text
mini_siem_agent   collects logs and system events
mini_siem_server  stores events, runs detection, exposes API
SQLite            stores events and alerts
web dashboard     visualizes events and alerts
scripts           provide portable demo execution
```
