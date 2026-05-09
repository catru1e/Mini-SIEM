# Mini-SIEM USB

Mini-SIEM USB is a portable Linux security monitoring project that demonstrates a compact SIEM-like pipeline.

The system collects security events from Linux logs, demo payloads, process snapshots, and file activity monitoring. Events are normalized, stored in SQLite, analyzed by detection rules and correlation logic, and displayed in a web dashboard with authentication and role-based access.

The project is designed to run from a portable directory, including a USB drive.

---

## Overview

Mini-SIEM USB includes:

- C++ agent for event collection
- C++ server for ingestion, storage, detection and API
- SQLite event and authentication storage
- JSON-based detection rules
- Event correlation
- Web dashboard
- User login and roles
- Portable demo launcher

Main pipeline:

```text
Linux logs / demo payloads / process events / file events
        |
        v
mini_siem_agent
        |
        v
mini_siem_server
        |
        v
SQLite storage
        |
        v
Detection rules + correlation
        |
        v
Alerts + dashboard
```

---

## Features

### Event Collection

- Demo auth log collection
- Real Linux `/var/log` collection
- Multiple log source support
- Per-file offset tracking
- Auth log parsing
- Syslog parsing
- Process snapshot monitoring
- File activity monitoring with inotify

### Event Processing

- Normalized security events
- Server-side event enrichment
- Event UUID generation
- Server receive timestamp
- Source type classification
- SQLite event storage

### Detection

- JSON-based rule engine
- Threshold rules
- Match rules
- Alert suppression
- Failed login detection
- Invalid user detection
- Sudo / privilege escalation detection
- Suspicious process detection
- File activity detection
- Event correlation

### Dashboard

- Login screen
- Role-based interface
- Overview dashboard
- Auth dashboard
- File activity dashboard
- Custom dashboard builder
- Dashboard library
- KPI cards
- Charts
- Recent events and alerts
- KQL-like search
- Raw / Fields / Packet inspection
- Copy buttons for JSON and raw blocks
- Horror red / cute pink themes

### User Management

- Admin, analyst and viewer roles
- Admin user management tab
- Create users
- Block and unblock users
- Reset passwords
- Users can change their own password
- Logout support

---

## User Roles

```text
admin
  Full access.
  Can manage users, rules and dashboards.

analyst
  Can investigate dashboards, events, alerts and rules.
  Cannot manage users.

viewer
  Read-only access to dashboards, events and alerts.
```

Visible dashboard tabs:

```text
admin    Dashboard / Events / Alerts / Rules / Users
analyst  Dashboard / Events / Alerts / Rules
viewer   Dashboard / Events / Alerts
```

---

## Project Structure

```text
mini-siem-usb/
|-- agent/          event collection and parsers
|-- server/         ingestion, storage, detection, auth and API
|-- common/         shared config loader
|-- config/         config, rules and dashboards
|-- data/           SQLite databases and agent state
|-- dist/           compiled binaries
|-- logs/           runtime logs and demo payloads
|-- scripts/        build, run and demo scripts
|-- third_party/    single-header dependencies
|-- web/            dashboard frontend
`-- README.md
```

Important files:

```text
config/config.json       main runtime config
config/rules.json        detection rules
config/dashboards.json   saved dashboards
data/events.db           events and alerts
data/auth.db             users and sessions
data/agent_state.json    saved log offsets
```

---

## Requirements

Install required packages:

```bash
sudo apt update
sudo apt install build-essential cmake sqlite3 libsqlite3-dev libsodium-dev
```

Used technologies:

```text
C++17
CMake
SQLite
cpp-httplib
nlohmann/json
libsodium
HTML / CSS / JavaScript
```

The frontend uses plain HTML, CSS and JavaScript.  
No npm, React or external frontend build system is required.

---

## Quick Start

From the project root:

```bash
./scripts/build.sh
./scripts/run_demo.sh
```

Then open the dashboard URL printed by the launcher.

Default URL:

```text
http://127.0.0.1:6969
```

Default demo login:

```text
username: admin
password: admin123
```

If the port was changed in `config/config.json`, open that port instead.

---

## Demo Launcher

The recommended way to run the project is:

```bash
./scripts/run_demo.sh
```

The launcher starts the server and the agent, shows runtime logs, and allows demo payload injection.

Controls:

```text
d  inject demo events
s  open server log
a  open agent log
q  stop everything and quit
```

Inside the log viewer:

```text
w  scroll up one line
s  scroll down one line
a  scroll up one page
d  scroll down one page
g  go to beginning
G  go to end
q  return to launcher
```

---

## Demo Payloads

Demo payload files can be injected into:

```text
logs/demo_auth.log
```

Example payloads:

```text
logs/bruteforce.txt
logs/sudo_test.txt
logs/full_attack.txt
```

Demo flow:

```text
demo payload
    |
    v
logs/demo_auth.log
    |
    v
agent
    |
    v
server
    |
    v
SQLite
    |
    v
dashboard alerts
```

---

## Supported Event Sources

Default log sources are configured in `config/config.json`.

```text
logs/demo_auth.log
/var/log/auth.log
/var/log/syslog
/var/log/kern.log
```

The agent also collects:

```text
process events from /proc
file activity events from watched paths
```

Some real Linux logs require elevated permissions.  
Because of that, the demo launcher may ask for the sudo password.

---

## Event Types

Authentication parser can produce:

```text
auth_failed
auth_success
auth_invalid_user
privilege_escalation
session_open
session_close
```

Syslog parser can produce:

```text
system_event
```

Process monitoring can produce:

```text
process_start
```

File monitoring can produce:

```text
file_created
file_modified
file_deleted
```

---

## Event Model

Example normalized event:

```json
{
  "event_id": "2f966077-5ed1-4276-b2e3-f9f0898a206b",
  "ts": "2026-05-09T11:38:12.706Z",
  "received_at": "2026-05-09T11:38:12.707Z",
  "host": "smth",
  "source": "auth.log",
  "source_type": "auth",
  "event_type": "auth_failed",
  "severity": "medium",
  "user": "root",
  "src_ip": "1.2.3.4",
  "raw": "Failed password for root from 1.2.3.4 port 51234 ssh2"
}
```

Supported source types:

```text
auth
syslog
process
file
```

---

## Detection Rules

Rules are stored in:

```text
config/rules.json
```

Supported rule types:

```text
threshold
match
```

Example threshold rule:

```json
{
  "id": "RULE_FAILED_LOGIN_IP",
  "enabled": true,
  "type": "threshold",
  "event_types": ["auth_failed"],
  "source_types": ["auth"],
  "group_by": "src_ip",
  "threshold": 5,
  "window_sec": 300,
  "suppress_sec": 60,
  "severity": "high",
  "title": "Brute force from IP",
  "description": "Detected {{count}} failed login attempts from IP {{group_key}}."
}
```

Rules can be viewed and managed from the dashboard.

---

## Correlation

Mini-SIEM also supports simple event correlation.

Example:

```text
multiple failed logins
        |
        v
successful login from same source
        |
        v
possible brute-force success alert
```

This makes the project closer to a SIEM instead of only a log viewer.

---

## Dashboard

Dashboard tabs:

```text
Dashboard
Events
Alerts
Rules
Users
```

Dashboard modes:

```text
Overview
Auth
File activity
Custom
```

The dashboard includes:

```text
KPI cards
event timeline
alerts by severity
events by source type
top IPs
top users
top file paths
recent events
recent alerts
custom dashboard builder
dashboard JSON preview
```

Events and alerts support KQL-like search.

Example event queries:

```text
event_type:auth_failed
severity:high
host:smth
source_type:syslog
raw:DENIED
user:root
ip:10.0.0.5
```

Example alert queries:

```text
severity:high
rule:RULE_FAILED_LOGIN_IP
host:smth
raw:Failed
group:root
```

Inspection buttons:

```text
Raw
Fields
Packet
```

---

## Themes

The dashboard supports two themes:

```text
Horror red
Cute pink
```

Theme can be changed from the login page and from the dashboard.

---

## Clean Reset

Reset runtime data while preserving saved log offsets:

```bash
./scripts/reset_demo.sh
```

This keeps:

```text
data/agent_state.json
```

Full reset including offsets:

```bash
./scripts/reset_demo.sh --with-state
```

Reset users and login data:

```bash
rm -f data/auth.db
```

After removing `data/auth.db`, the first admin user will be created again on next launch.

---

## API Checks

Health check:

```bash
curl http://127.0.0.1:6969/health
```

Expected output:

```text
OK
```

Login and save session cookie:

```bash
curl -i -c /tmp/mini_siem_cookie.txt \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}' \
  http://127.0.0.1:6969/api/auth/login
```

Check current user:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  http://127.0.0.1:6969/api/auth/me
```

Recent events:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  "http://127.0.0.1:6969/api/events?limit=20"
```

Recent alerts:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  "http://127.0.0.1:6969/api/alerts?limit=20"
```

Rules:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  "http://127.0.0.1:6969/api/rules"
```

Dashboard stats:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  "http://127.0.0.1:6969/api/stats"
```

---

## USB Execution Note

If the USB drive is mounted with `noexec`, Linux will not allow binaries or scripts to run directly from it.

Typical error:

```text
Permission denied
```

In this case, remount the USB drive with the `exec` option or copy the project to an executable filesystem.

Example:

```bash
cd /

DEV=$(findmnt -no SOURCE /media/ctrl/MINI-SIEM)

sudo umount /media/ctrl/MINI-SIEM

sudo mkdir -p /mnt/mini-siem-usb

sudo mount -o uid=$(id -u),gid=$(id -g),fmask=0022,dmask=0022,exec "$DEV" /mnt/mini-siem-usb
```

Then:

```bash
cd /mnt/mini-siem-usb/mini-siem-usb
./scripts/build.sh
./scripts/run_demo.sh
```

---

## Final Demonstration Flow

Recommended final demo:

```bash
cd /mnt/mini-siem-usb/mini-siem-usb
./scripts/build.sh
./scripts/run_demo.sh
```

Then:

```text
1. Open the dashboard URL.
2. Login as admin.
3. Show Dashboard, Events, Alerts, Rules and Users tabs.
4. Create analyst and viewer users.
5. Show role differences.
6. Inject bruteforce demo events.
7. Show auth_failed events.
8. Show generated alerts.
9. Open Auth dashboard.
10. Open Events and Alerts search.
11. Inspect Raw / Fields / Packet views.
12. Show detection rules.
13. Create or disable a test rule.
14. Inject sudo demo events.
15. Show privilege escalation alerts.
16. Show process or file activity events.
17. Open Custom dashboard builder.
18. Show theme switching.
19. Open server and agent logs from launcher.
20. Quit with q.
```

---

## Troubleshooting

### Dashboard does not open

Check that `run_demo.sh` is still running and server started correctly.

```bash
curl http://127.0.0.1:6969/health
```

### Login does not work

Reset authentication database:

```bash
rm -f data/auth.db
./scripts/run_demo.sh
```

### Events are not appearing

Check agent log from the launcher:

```text
a
```

Also check that demo events were injected:

```text
d
```

### Real Linux logs are not collected

Some logs require sudo permissions.

Check whether the files exist:

```bash
ls -l /var/log/auth.log /var/log/syslog /var/log/kern.log
```

### Old logs appear again

The agent offset file may have been reset:

```text
data/agent_state.json
```

If this file is removed, the agent can reread logs from the beginning.

---

## Educational Value

Mini-SIEM USB demonstrates important SIEM concepts:

```text
event collection
normalization
ingestion
storage
rule-based detection
alerting
correlation
dashboards
role-based access
portable execution
```

It is intentionally compact, but includes enough functionality to look and behave like a small real-world security monitoring system.
