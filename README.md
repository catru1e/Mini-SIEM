# Mini-SIEM USB

Mini-SIEM USB is a compact Security Information and Event Management system for Linux operating system security monitoring.

The system collects Linux security events from demo log files, real Linux log files, process snapshots, and file activity monitoring. Events are normalized by the agent, enriched by the server, stored in SQLite, checked by a JSON-based detection rule engine, processed by correlation logic, protected by authentication and displayed in a web dashboard.

The project is designed to run as a portable directory from a USB drive.

---

## Features

### Core SIEM Pipeline

- Portable USB execution
- C++ agent and C++ server
- SQLite-based storage
- Real Linux log collection
- Demo log injection
- Multiple log source support
- Per-file offset tracking
- Authentication log parsing
- Universal syslog parsing
- Process monitoring through `/proc`
- File activity monitoring through inotify
- Server-side event enrichment
- Event UUID generation
- Server-side receive timestamp
- Source type classification
- Detection alerts
- Event correlation
- REST API
- SSE endpoint for realtime-style updates

### Detection and Rules

- JSON-based detection rule engine
- Rules stored in `config/rules.json`
- Web-based rule management
- Threshold-based detection rules
- Match-based detection rules
- Alert suppression support
- Failed login detection
- Invalid user detection
- Successful login detection
- Sudo / privilege escalation detection
- Suspicious process detection
- Sensitive file activity detection
- Rule validation and JSON preview in UI

### Dashboards

- SOC-style web dashboard
- KPI cards and charts
- Events over time
- Alerts by severity
- Events by source type
- Top source IPs
- Top users
- Top file paths
- Recent events
- Recent alerts
- Overview dashboard
- Auth dashboard
- File activity dashboard
- Custom dashboard builder
- Dashboard library
- Dashboard JSON preview
- Scrollable dashboard widgets
- KQL-like event and alert search
- Raw / Fields / Packet inspection
- Copy button for Raw / Packet / JSON blocks
- Manual refresh and optional auto-refresh
- Horror red / cute pink themes

### Authentication and User Management

- Login page
- HttpOnly session cookie
- Local users stored in `data/auth.db`
- Password hashing with Argon2id through libsodium
- Role-based access control
- Admin / analyst / viewer roles
- Admin user management tab
- Create users
- Block and unblock users
- Reset user passwords
- Temporary password flow
- Users can change their own password
- Forced password change after admin-created or reset password
- Logout support
- Agent authentication with Bearer token for `/ingest`

---

## Purpose

This project demonstrates a compact SIEM-like pipeline:

```text
collection -> normalization -> ingestion -> storage -> detection -> correlation -> dashboard -> authentication
```

It is intended as an educational Mini-SIEM system for operating system security monitoring.

---

## Architecture

```text
Demo payloads / Linux logs / process events / file events
        |
        v
mini_siem_agent
        |
        | HTTP JSON events
        | Authorization: Bearer <agent_token>
        v
mini_siem_server
        |
        | event_id + received_at + source_type
        v
SQLite events database
        |
        v
JSON rule engine + correlation
        |
        v
Alerts database records
        |
        v
Protected REST API
        |
        v
Authenticated Web Dashboard
```

Authentication is handled separately from event ingestion:

```text
Browser user login
        |
        v
/api/auth/login
        |
        v
HttpOnly cookie session
        |
        v
Protected dashboard APIs
```

Agent ingestion uses a separate token:

```text
mini_siem_agent
        |
        | Authorization: Bearer <MINI_SIEM_AGENT_TOKEN>
        v
POST /ingest
```

---

## Project Structure

```text
mini-siem-usb/
|-- agent/
|   `-- src/
|       |-- main.cpp
|       |-- http_client.cpp
|       |-- log_reader.cpp
|       |-- state_store.cpp
|       |-- auth_parser.cpp
|       |-- syslog_parser.cpp
|       |-- process_snapshot.cpp
|       `-- file_monitor.cpp
|
|-- server/
|   `-- src/
|       |-- main.cpp
|       |-- db_sqlite.cpp
|       |-- detect.cpp
|       |-- correlate.cpp
|       |-- auth_db.cpp
|       `-- session_store.cpp
|
|-- common/
|   |-- include/config.hpp
|   `-- src/config.cpp
|
|-- config/
|   |-- config.json
|   |-- rules.json
|   `-- dashboards.json
|
|-- data/
|   |-- events.db
|   |-- auth.db
|   `-- agent_state.json
|
|-- dist/
|   |-- mini_siem_agent
|   `-- mini_siem_server
|
|-- logs/
|   |-- demo_auth.log
|   |-- bruteforce.txt
|   |-- sudo_test.txt
|   |-- agent.log
|   `-- server.log
|
|-- scripts/
|   |-- build.sh
|   |-- run_demo.sh
|   |-- run_server.sh
|   |-- run_agent.sh
|   |-- reset_demo.sh
|   |-- push_demo_events.sh
|   `-- run_collect.sh
|
|-- third_party/
|   |-- httplib.h
|   `-- json.hpp
|
|-- web/
|   |-- index.html
|   |-- app.js
|   `-- style.css
|
|-- CMakeLists.txt
|-- Dockerfile
|-- docker-compose.yml
`-- README.md
```

---

## Dependencies

Required packages:

```bash
sudo apt update
sudo apt install build-essential cmake sqlite3 libsqlite3-dev libsodium-dev
```

Main technologies:

```text
C++17
CMake
SQLite
cpp-httplib
nlohmann/json
libsodium Argon2id
HTML
CSS
Vanilla JavaScript
```

The frontend does not require npm, React, build tooling, or external CDN dependencies.

---

## Configuration

Main configuration file:

```text
config/config.json
```

Example:

```json
{
  "server": {
    "host": "127.0.0.1",
    "port": 6969,
    "db_path": "data/events.db"
  },
  "agent": {
    "log_paths": [
      "logs/demo_auth.log",
      "/var/log/auth.log",
      "/var/log/syslog",
      "/var/log/kern.log"
    ],
    "state_path": "data/agent_state.json",
    "agent_log_path": "logs/agent.log",
    "ssh_watch_path": "~/.ssh"
  },
  "auth": {
    "db_path": "data/auth.db",
    "session_cookie": "mini_siem_session",
    "session_ttl_seconds": 86400
  },
  "dashboard": {
    "events_limit_default": 100,
    "alerts_limit_default": 100,
    "events_limit_max": 0,
    "alerts_limit_max": 0
  },
  "paths": {
    "data_dir": "data",
    "logs_dir": "logs"
  }
}
```

Important runtime files:

```text
data/events.db          events and alerts database
data/auth.db            users and sessions database
data/agent_state.json   per-file log offsets and agent state
logs/agent.log          agent runtime log
logs/server.log         server runtime log
```

---

## Environment Variables

The project uses environment variables for sensitive runtime values.

Recommended variables:

```bash
export MINI_SIEM_ADMIN_USER=admin
export MINI_SIEM_ADMIN_PASSWORD=admin123
export MINI_SIEM_AGENT_TOKEN=dev-agent-token
```

Purpose:

```text
MINI_SIEM_ADMIN_USER      initial admin username
MINI_SIEM_ADMIN_PASSWORD  initial admin password
MINI_SIEM_AGENT_TOKEN     token used by the agent for /ingest
```

If there are no users in `data/auth.db`, the server creates the first admin automatically.

For demo purposes, `admin / admin123` can be used locally. For any real or remote usage, change these values.

---

## Build

From the project root:

```bash
./scripts/build.sh
```

Expected binaries:

```text
dist/mini_siem_server
dist/mini_siem_agent
```

Manual CMake build:

```bash
rm -rf build
mkdir build
cd build
cmake ..
make
cd ..
```

---

## Quick Start

From the project root:

```bash
export MINI_SIEM_ADMIN_USER=admin
export MINI_SIEM_ADMIN_PASSWORD=admin123
export MINI_SIEM_AGENT_TOKEN=dev-agent-token

./scripts/build.sh
./scripts/run_demo.sh
```

The launcher starts:

```text
mini_siem_server
mini_siem_agent
```

The server runs as the current user.

The agent may run with `sudo` because real Linux logs such as `/var/log/auth.log`, `/var/log/syslog`, and `/var/log/kern.log` usually require elevated permissions.

Open the dashboard:

```text
http://127.0.0.1:6969
```

Login with:

```text
username: admin
password: admin123
```

If your `config/config.json` uses another port, open that port instead.

---

## Manual Run

Run server:

```bash
MINI_SIEM_ADMIN_USER=admin \
MINI_SIEM_ADMIN_PASSWORD=admin123 \
MINI_SIEM_AGENT_TOKEN=dev-agent-token \
./dist/mini_siem_server
```

Run agent in another terminal:

```bash
MINI_SIEM_AGENT_TOKEN=dev-agent-token \
./dist/mini_siem_agent
```

If the agent must read protected Linux logs:

```bash
sudo MINI_SIEM_AGENT_TOKEN=dev-agent-token ./dist/mini_siem_agent
```

---

## Authentication

Mini-SIEM has local authentication.

Authentication database:

```text
data/auth.db
```

Tables:

```sql
CREATE TABLE users (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username TEXT NOT NULL UNIQUE,
  password_hash TEXT NOT NULL,
  role TEXT NOT NULL DEFAULT 'viewer',
  enabled INTEGER NOT NULL DEFAULT 1,
  password_change_required INTEGER NOT NULL DEFAULT 0,
  created_at TEXT NOT NULL,
  updated_at TEXT NOT NULL
);

CREATE TABLE sessions (
  id TEXT PRIMARY KEY,
  username TEXT NOT NULL,
  role TEXT NOT NULL,
  created_at TEXT NOT NULL,
  expires_at TEXT NOT NULL
);
```

Passwords are never stored in plaintext.

Mini-SIEM stores only password hashes generated with Argon2id through libsodium.

Session is stored as an HttpOnly cookie:

```text
mini_siem_session=<session_id>
HttpOnly
SameSite=Lax
Path=/
```

This is safer than storing the session in localStorage because JavaScript cannot read HttpOnly cookies.

---

## Roles

Mini-SIEM has three user roles.

```text
admin
analyst
viewer
```

Access model:

```text
admin:
  - view dashboards
  - view events
  - view alerts
  - view rules
  - create/edit/delete rules
  - create/edit/delete dashboards
  - create users
  - block/unblock users
  - reset user passwords
  - delete users

analyst:
  - view dashboards
  - view events
  - view alerts
  - view rules
  - investigate alerts and rules
  - cannot manage users
  - cannot modify rules

viewer:
  - view dashboards
  - view events
  - view alerts
  - read-only access
  - cannot view rules tab
  - cannot manage users
```

---

## User Management

The Admin user can manage local Mini-SIEM users from the `Users` tab.

Supported actions:

```text
create user
change user role
block user
unblock user
reset password
delete user
```

When an admin creates a user, the password is temporary.

After first login, the user must change the password before using the dashboard.

When an admin resets a password, the new password is also temporary.

Temporary password flow:

```text
admin creates or resets password
        |
        v
user logs in with temporary password
        |
        v
password change modal opens
        |
        v
user sets a new password
        |
        v
dashboard becomes available
```

Users can also change their own password from the session card.

---

## Protected API Model

Browser APIs require a valid login session.

Agent ingestion requires a separate Bearer token.

```text
/api/events       login required
/api/alerts       login required
/api/stats        login required
/api/rules        analyst/admin for viewing
/api/rules        admin for create/update/delete
/api/dashboards   login required for viewing
/api/dashboards   admin for create/update/delete
/api/users        admin only
/stream           login required
/ingest           agent token required
```

`/ingest` does not use browser login cookies.

It uses:

```text
Authorization: Bearer <MINI_SIEM_AGENT_TOKEN>
```

This keeps machine authentication separate from user authentication.

---

## Authentication API

Login:

```bash
curl -i -c /tmp/mini_siem_cookie.txt \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}' \
  http://127.0.0.1:6969/api/auth/login
```

Current user:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  http://127.0.0.1:6969/api/auth/me
```

Logout:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  -X POST \
  http://127.0.0.1:6969/api/auth/logout
```

Change own password:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  -H "Content-Type: application/json" \
  -d '{"old_password":"admin123","new_password":"newpass123"}' \
  http://127.0.0.1:6969/api/auth/change-password
```

---

## Users API

Admin-only endpoints:

```text
GET    /api/users
POST   /api/users
PUT    /api/users/:username
DELETE /api/users/:username
POST   /api/users/:username/password
```

List users:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  http://127.0.0.1:6969/api/users
```

Create user:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  -H "Content-Type: application/json" \
  -d '{"username":"analyst1","password":"temp123","role":"analyst","enabled":true}' \
  http://127.0.0.1:6969/api/users
```

Block user:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  -X PUT \
  -H "Content-Type: application/json" \
  -d '{"role":"analyst","enabled":false}' \
  http://127.0.0.1:6969/api/users/analyst1
```

Unblock user:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  -X PUT \
  -H "Content-Type: application/json" \
  -d '{"role":"analyst","enabled":true}' \
  http://127.0.0.1:6969/api/users/analyst1
```

Reset user password:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  -H "Content-Type: application/json" \
  -d '{"password":"newtemp123"}' \
  http://127.0.0.1:6969/api/users/analyst1/password
```

Delete user:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  -X DELETE \
  http://127.0.0.1:6969/api/users/analyst1
```

---

## Ingestion API

The agent sends events to:

```text
POST /ingest
```

This endpoint requires the agent token.

Without token:

```bash
curl -i \
  -H "Content-Type: application/json" \
  -d '{"event_type":"test","source":"manual"}' \
  http://127.0.0.1:6969/ingest
```

Expected result:

```text
401 Unauthorized
```

With token:

```bash
curl -i \
  -H "Authorization: Bearer dev-agent-token" \
  -H "Content-Type: application/json" \
  -d '{"event_type":"test","source":"manual","raw":"hello"}' \
  http://127.0.0.1:6969/ingest
```

Expected result:

```text
200 OK
```

---

## Supported Log Sources

The agent reads multiple log sources configured in `config/config.json`.

Default sources:

```text
logs/demo_auth.log
/var/log/auth.log
/var/log/syslog
/var/log/kern.log
```

`logs/demo_auth.log` is used for controlled demo payloads.

Real Linux logs usually require elevated permissions:

```text
/var/log/auth.log
/var/log/syslog
/var/log/kern.log
```

The agent stores offsets separately for every file in:

```text
data/agent_state.json
```

Example state:

```json
{
  "files": {
    "logs/demo_auth.log": 524,
    "/var/log/auth.log": 31717,
    "/var/log/syslog": 15577,
    "/var/log/kern.log": 10750
  }
}
```

This allows the agent to continue reading from the last saved position instead of rereading the same logs every time.

---

## Event Parsing

The agent uses different parsers depending on the log source.

```text
auth logs      -> auth_parser
other syslogs  -> syslog_parser
```

Authentication logs produce normalized security events:

```text
auth_failed
auth_success
auth_invalid_user
privilege_escalation
session_open
session_close
```

Generic system logs produce normalized system events:

```json
{
  "source": "syslog",
  "event_type": "system_event",
  "program": "kernel",
  "severity": "low",
  "raw": "..."
}
```

If an auth log line is not recognized, the agent can still send it as a raw log event.

---

## Process Monitoring

The agent can capture process snapshots through `/proc`.

Detected process events use:

```text
source: proc
event_type: process_start
source_type: process
```

Example:

```json
{
  "event_type": "process_start",
  "source": "proc",
  "source_type": "process",
  "process_name": "nc",
  "pid": 1234,
  "cmdline": "nc -lvp 4444",
  "severity": "info"
}
```

Detection rules can match suspicious or blacklisted process names.

---

## File Activity Monitoring

The agent monitors configured file paths through inotify.

Default monitored path:

```text
~/.ssh
```

Example file event types:

```text
file_created
file_modified
file_deleted
```

File events use:

```text
source_type: file
```

This makes it possible to detect suspicious changes in sensitive directories.

---

## Ingestion Pipeline

The server enriches every incoming event before storing it.

Server-side fields:

```text
event_id     unique UUID generated by the server
received_at  server receive timestamp
source_type  normalized event source category
```

Supported `source_type` values:

```text
auth
syslog
process
file
```

Example event:

```json
{
  "event_id": "2f966077-5ed1-4276-b2e3-f9f0898a206b",
  "ts": "2026-05-09T11:38:12.706Z",
  "received_at": "2026-05-09T11:38:12.707Z",
  "host": "smth",
  "source": "kern.log",
  "source_type": "syslog",
  "event_type": "system_event",
  "severity": "medium",
  "program": "kernel",
  "raw": "..."
}
```

---

## SQLite Storage

Main event database:

```text
data/events.db
```

Main tables:

```text
events
alerts
```

Events table stores normalized event columns and the full JSON packet.

Alerts table stores alerts generated by detection and correlation logic.

Authentication database:

```text
data/auth.db
```

Main tables:

```text
users
sessions
```

---

## Detection Rule Engine

Detection rules are stored in JSON instead of being hardcoded only in C++.

Rules file:

```text
config/rules.json
```

The rule engine supports:

```text
enabled       enable or disable a rule
type          threshold or match
event_types   list of event types
source_types  list of source types
group_by      field used for grouping
threshold     event count needed to trigger
window_sec    detection time window
suppress_sec  alert suppression window
severity      alert severity
title         alert title
description   alert description template
conditions    additional field checks
```

Supported rule types:

```text
threshold  count events over a time window
match      alert when one event matches conditions
```

Supported condition operators:

```text
equals
not_equals
contains
contains_any
in
starts_with
ends_with
exists
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
  "description": "Detected {{count}} failed login attempts from IP {{group_key}} within {{window_sec}} seconds."
}
```

Example match rule:

```json
{
  "id": "RULE_KERNEL_ERROR",
  "enabled": true,
  "type": "match",
  "event_types": ["system_event"],
  "source_types": ["syslog"],
  "group_by": "program",
  "suppress_sec": 60,
  "severity": "medium",
  "title": "Kernel or system error message",
  "description": "Detected system error from {{group_key}}.",
  "conditions": [
    {
      "field": "severity",
      "op": "in",
      "values": ["medium", "high"]
    }
  ]
}
```

Example alert generated by the rule engine:

```json
{
  "rule_id": "RULE_FAILED_LOGIN_IP",
  "rule_name": "RULE_FAILED_LOGIN_IP",
  "severity": "high",
  "title": "Brute force from IP",
  "group_by": "src_ip",
  "group_key": "1.2.3.4",
  "count": 5,
  "threshold": 5,
  "window_seconds": 300,
  "suppress_seconds": 60,
  "source_type": "auth"
}
```

---

## Correlation

The project includes correlation logic for attack sequences.

Example:

```text
multiple auth_failed events
        |
        v
auth_success from the same source
        |
        v
Possible brute-force success alert
```

Correlation helps the system behave more like a SIEM instead of only a log viewer.

---

## Rules Management API

Detection rules API:

```text
GET     /api/rules
POST    /api/rules
PUT     /api/rules/:id
DELETE  /api/rules/:id
```

Access:

```text
GET rules              analyst/admin
create/update/delete   admin
```

Get rules:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  http://127.0.0.1:6969/api/rules
```

Delete rule:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  -X DELETE \
  http://127.0.0.1:6969/api/rules/RULE_TEST_API
```

The API edits `config/rules.json` and reloads the detection engine after changes.

---

## Dashboard Management

Dashboards can be stored in:

```text
config/dashboards.json
```

Dashboard API:

```text
GET     /api/dashboards
POST    /api/dashboards
PUT     /api/dashboards/:id
DELETE  /api/dashboards/:id
```

The Custom dashboard builder supports:

```text
new dashboard
duplicate dashboard
save dashboard
delete dashboard
dashboard title
dashboard description
layout columns
widget builder
widget filters
dashboard JSON preview
dashboard library
```

Supported custom widget types:

```text
KPI events
KPI alerts
KPI high/critical alerts
Top IPs
Top users
Top file paths
Alerts trend
Event type distribution
Severity distribution
Source type distribution
Recent events
Recent alerts
```

---

## Web Dashboard

The web dashboard is a SOC-style investigation console.

Main tabs:

```text
Dashboard
Events
Alerts
Rules
Users
```

Tabs visible by role:

```text
admin:
  Dashboard
  Events
  Alerts
  Rules
  Users

analyst:
  Dashboard
  Events
  Alerts
  Rules

viewer:
  Dashboard
  Events
  Alerts
```

Dashboard modes:

```text
Overview
Auth
File activity
Custom
```

The Dashboard tab shows:

```text
total events
total alerts
high / critical alerts
active rules
events over time
alerts by severity
events by source type
top source IPs
top users
alerts trend
recent events
recent alerts
```

The Auth dashboard shows:

```text
auth events
failed logins
successful logins
invalid users
top attacking IPs
top targeted users
auth event types
auth alerts
recent auth events
```

The File activity dashboard shows:

```text
file events
created files
modified files
deleted files
top file paths
file actions
file severity
file alerts
recent file events
```

---

## KQL-like Search

The Events and Alerts sections support KQL-like search.

Example event queries:

```text
event_type:auth_failed
severity:high
host:smth
source_type:syslog
raw:DENIED
user:root
ip:10.0.0.5
program:sshd
```

Example alert queries:

```text
severity:high
rule:RULE_KERNEL_ERROR
host:smth
raw:DENIED
group:kernel
source_type:syslog
event_type:system_event
```

Multiple query terms work as AND conditions.

Example:

```text
event_type:system_event raw:error
```

Quoted values are supported.

Example:

```text
raw:"Failed password"
```

---

## Event and Alert Inspection

Events support:

```text
Raw
Fields
Packet
```

Alerts support:

```text
Description
Source raw
Packet
```

Raw / Packet / JSON blocks include a copy button for easier investigation and reporting.

---

## Themes

The dashboard supports two themes:

```text
Horror red
Cute pink
```

The selected theme is saved in browser local storage.

The login page also supports theme switching.

---

## Demo Payloads

Prepared demo payload files can be injected into `logs/demo_auth.log`.

Example payload files:

```text
logs/bruteforce.txt
logs/sudo_test.txt
logs/full_attack.txt
```

Demo pipeline:

```text
payload file
    |
    v
scripts/push_demo_events.sh
    |
    v
logs/demo_auth.log
    |
    v
mini_siem_agent
    |
    v
mini_siem_server
    |
    v
SQLite database
    |
    v
Web dashboard
```

---

## Demo Launcher Controls

When `run_demo.sh` is running:

```text
d  inject demo events from selected file
s  open full server log
a  open full agent log
q  stop server, stop agent, and quit
```

Inside the log viewer:

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

## Clean Reset

To clean the database and runtime logs while preserving saved log offsets:

```bash
./scripts/reset_demo.sh
```

This keeps:

```text
data/agent_state.json
```

Preserving this file is important because it stores offsets for every log source.

To fully reset everything, including saved offsets:

```bash
./scripts/reset_demo.sh --with-state
```

Use `--with-state` only when you intentionally want the agent to reread logs from the beginning.

Authentication database can be reset manually:

```bash
rm -f data/auth.db
```

After removing `data/auth.db`, the server will create the initial admin again on next start.

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

Login first:

```bash
curl -i -c /tmp/mini_siem_cookie.txt \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}' \
  http://127.0.0.1:6969/api/auth/login
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

Detection rules:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  "http://127.0.0.1:6969/api/rules"
```

Dashboard stats:

```bash
curl -b /tmp/mini_siem_cookie.txt \
  "http://127.0.0.1:6969/api/stats"
```

Pretty-print recent alerts:

```bash
curl -s -b /tmp/mini_siem_cookie.txt \
  "http://127.0.0.1:6969/api/alerts?limit=10" | python3 -m json.tool
```

Pretty-print rules:

```bash
curl -s -b /tmp/mini_siem_cookie.txt \
  "http://127.0.0.1:6969/api/rules" | python3 -m json.tool
```

Pretty-print dashboard stats:

```bash
curl -s -b /tmp/mini_siem_cookie.txt \
  "http://127.0.0.1:6969/api/stats" | python3 -m json.tool
```

If your `config/config.json` uses another port, replace `6969` with that port.

---

## Main Components

### mini_siem_agent

The agent collects security-related events from the operating system.

It supports:

```text
multiple configured log files
demo auth log monitoring
real Linux log monitoring
auth log parsing
universal syslog parsing
raw syslog and kernel log collection
process snapshot monitoring
file activity monitoring
per-file offset state storage
HTTP event sending
Bearer token authentication for /ingest
```

### mini_siem_server

The server receives, stores, analyzes, and serves events.

It supports:

```text
event ingestion
agent token validation
server-side event enrichment
UUID event identifiers
receive timestamps
source type classification
SQLite event storage
JSON-based rule engine
threshold detection rules
match detection rules
alert suppression
rules management API
alert generation
event correlation
authentication database
session store
user management API
role-based API protection
dashboard statistics API
SSE endpoint
static dashboard serving
```

### Web Dashboard

The dashboard displays and manages:

```text
login
logout
own password change
temporary password change flow
events
alerts
rules
users
dashboards
custom dashboards
KPI cards
charts
event timeline
severity distribution
source type distribution
KQL-like search
raw log inspection
normalized fields inspection
full packet inspection
manual refresh
auto-refresh
theme switching
```

---

## Security Notes

This project is designed for local educational usage.

Current security features:

```text
Argon2id password hashing
HttpOnly session cookies
SameSite=Lax cookies
role-based access control
separate agent token for ingestion
temporary password flow
user blocking
password reset
```

For remote deployment, Mini-SIEM should be served behind HTTPS.

Recommended reverse proxies:

```text
nginx
caddy
traefik
```

Do not expose the server directly to the public internet without HTTPS, stronger secret handling, and production hardening.

---

## USB Execution Note

If the USB drive is mounted with `noexec`, Linux will not allow scripts or binaries to run directly from the USB drive.

Typical error:

```text
Permission denied
```

In this case, remount the USB drive with the `exec` option.

Example:

```bash
cd /

DEV=$(findmnt -no SOURCE /media/ctrl/MINI-SIEM)

sudo umount /media/ctrl/MINI-SIEM

sudo mkdir -p /mnt/mini-siem-usb

sudo mount -o uid=$(id -u),gid=$(id -g),fmask=0022,dmask=0022,exec "$DEV" /mnt/mini-siem-usb
```

Then go to the project directory:

```bash
cd /mnt/mini-siem-usb/mini-siem-usb
```

If the project path changes after remounting, rebuild from a clean build directory:

```bash
rm -rf build
./scripts/build.sh
```

---

## Final Demonstration Flow

Recommended final demo:

```bash
cd /mnt/mini-siem-usb/mini-siem-usb

export MINI_SIEM_ADMIN_USER=admin
export MINI_SIEM_ADMIN_PASSWORD=admin123
export MINI_SIEM_AGENT_TOKEN=dev-agent-token

./scripts/build.sh
./scripts/run_demo.sh
```

Then:

```text
1. Enter sudo password when requested.
2. Open the dashboard URL printed by the launcher.
3. Login as admin.
4. Show the Session card and Logout button.
5. Show Dashboard / Events / Alerts / Rules / Users tabs.
6. Create analyst and viewer users.
7. Show that admin can block/unblock users and reset passwords.
8. Login as a newly created user and show temporary password change modal.
9. Show role difference:
   admin   -> Rules + Users
   analyst -> Rules only, no Users
   viewer  -> Dashboard / Events / Alerts only
10. Show that real Linux logs are collected.
11. Show events with event_id, received_at and source_type.
12. Show that /ingest requires Authorization: Bearer token.
13. Show that rules are loaded from config/rules.json.
14. Open Overview dashboard and show KPI cards, charts, recent events and recent alerts.
15. Open Auth dashboard and show failed logins / successful logins / invalid users.
16. Open File activity dashboard and show file events.
17. Open Custom dashboard builder and show dashboard library.
18. Open Events and Alerts tabs and demonstrate KQL-like search.
19. Use Raw / Fields / Packet inspection.
20. Open the Detection Rules section.
21. Create a test rule from the web interface.
22. Press d in the launcher.
23. Choose logs/bruteforce.txt.
24. Show failed login events.
25. Show alerts generated by the rule engine.
26. Edit or disable the test rule.
27. Delete the test rule.
28. Press d again.
29. Choose logs/sudo_test.txt.
30. Show privilege escalation events and sudo alerts.
31. Show system_event entries from syslog or kern.log.
32. Show rule-based syslog alerts.
33. Press s to open the full server log.
34. Press a to open the full agent log.
35. Press q to stop everything.
```

---

## Troubleshooting

### Login does not work

Remove auth database and restart server:

```bash
rm -f data/auth.db
```

Then start server with admin environment variables again:

```bash
MINI_SIEM_ADMIN_USER=admin \
MINI_SIEM_ADMIN_PASSWORD=admin123 \
MINI_SIEM_AGENT_TOKEN=dev-agent-token \
./dist/mini_siem_server
```

### Agent cannot send events

Check that server and agent use the same token:

```bash
echo $MINI_SIEM_AGENT_TOKEN
```

Check `/ingest` manually:

```bash
curl -i \
  -H "Authorization: Bearer dev-agent-token" \
  -H "Content-Type: application/json" \
  -d '{"event_type":"test","source":"manual","raw":"hello"}' \
  http://127.0.0.1:6969/ingest
```

### Protected API returns 401

Login and use cookie file:

```bash
curl -i -c /tmp/mini_siem_cookie.txt \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}' \
  http://127.0.0.1:6969/api/auth/login
```

Then call protected APIs with:

```bash
curl -b /tmp/mini_siem_cookie.txt http://127.0.0.1:6969/api/auth/me
```

### Real Linux logs are not collected

Real logs may require sudo:

```bash
sudo MINI_SIEM_AGENT_TOKEN=dev-agent-token ./dist/mini_siem_agent
```

Also check that the files exist:

```bash
ls -l /var/log/auth.log /var/log/syslog /var/log/kern.log
```

Some Linux distributions may use different log files or `journald`.

### USB execution gives Permission denied

The USB drive may be mounted with `noexec`.

Remount with `exec` or copy the project to an executable filesystem.

### Repeated old logs appear again

The agent offset state may have been deleted.

Check:

```text
data/agent_state.json
```

If the file is removed, the agent may reread logs from the beginning.

---

## Educational Value

Mini-SIEM USB demonstrates the main ideas behind real SIEM systems:

```text
event collection
normalization
ingestion
storage
detection rules
alerting
correlation
dashboards
role-based access control
user management
secure password storage
machine-to-server token authentication
```

It is intentionally compact and understandable, but still includes many enterprise-style concepts in a local portable project.
