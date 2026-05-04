# Mini-SIEM USB

Mini-SIEM USB is a simplified Security Information and Event Management system for Linux operating system security monitoring.

The system collects Linux security events from demo log files, real Linux log files, process snapshots, and file activity monitoring. Events are normalized, sent to a local HTTP server, stored in SQLite, processed by detection and correlation logic, and displayed in a web dashboard.

The project is designed to run as a portable directory from a USB drive.

---

## Features

- Portable USB execution
- Unified launcher through `scripts/run_demo.sh`
- Demo log injection
- Real Linux `/var/log` collection
- Multiple log source support
- Per-file offset tracking
- Authentication log parsing
- Universal syslog parsing
- Failed login detection
- Successful login detection
- Sudo / privilege escalation detection
- Process monitoring through `/proc`
- File activity monitoring through inotify
- SQLite event storage
- Detection alerts
- Event correlation
- REST API
- SSE realtime dashboard updates
- Web dashboard

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

Real Linux logs such as `/var/log/auth.log`, `/var/log/syslog`, and `/var/log/kern.log` usually require elevated permissions. Because of that, `scripts/run_demo.sh` asks for the sudo password once and starts the agent with the required permissions.

---

## Event Parsing

The agent uses different parsers depending on the log source.

```text
auth logs       -> auth_parser
other syslogs   -> syslog_parser
```

Authentication logs produce normalized security events such as:

```text
failed_login
accepted_login
invalid_user
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

This allows the SIEM to understand more than only `auth.log`.

---

## Architecture

```text
Demo payloads / Linux logs / system sources
        |
        v
mini_siem_agent
        |
        | HTTP JSON events
        v
mini_siem_server
        |
        v
SQLite database
        |
        v
Detection + correlation
        |
        v
REST API / SSE
        |
        v
Web dashboard
```

---

## Project Structure

```text
mini-siem-usb/
├── agent/          # event collection, parsers, process/file monitoring
├── server/         # HTTP API, SQLite storage, detection, correlation
├── common/         # shared configuration loader
├── config/         # runtime configuration
├── data/           # SQLite database and agent state
├── dist/           # compiled binaries
├── exports/        # exported data, if needed
├── logs/           # runtime logs and demo payloads
├── scripts/        # build, run, demo and reset scripts
├── third_party/    # external single-header dependencies
├── web/            # dashboard frontend
└── README.md       # project overview and run guide
```

---

## Quick Start

From the project root:

```bash
./scripts/build.sh
./scripts/run_demo.sh
```

The launcher starts:

```text
mini_siem_server
mini_siem_agent
```

The server runs as the current user.

The agent runs with `sudo` so it can read protected real Linux log files from `/var/log`.

Then open the dashboard:

```text
http://127.0.0.1:6767
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

Preserving this file is important because it stores offsets for every log source. Without it, the agent will reread real Linux logs from the beginning.

To fully reset everything, including saved offsets:

```bash
./scripts/reset_demo.sh --with-state
```

Use `--with-state` only when you intentionally want the agent to reread logs from the beginning.

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

## Demo Payloads

Prepared demo payload files can be injected into `logs/demo_auth.log`.

Example payload files:

```text
logs/demo_payload.txt
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

## Real Linux Log Collection

The same launcher also collects real Linux logs.

Current real log sources:

```text
/var/log/auth.log
/var/log/syslog
/var/log/kern.log
```

Because these files are protected by Linux permissions, the launcher asks for the sudo password once.

The agent stores offsets separately for every file in:

```text
data/agent_state.json
```

Example state format:

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

## API Checks

Health check:

```bash
curl http://127.0.0.1:6767/health
```

Expected output:

```text
OK
```

Recent events:

```bash
curl "http://127.0.0.1:6767/api/events?limit=20"
```

Recent alerts:

```bash
curl "http://127.0.0.1:6767/api/alerts?limit=20"
```

---

## Main Components

### mini_siem_agent

The agent collects security-related events from the operating system.

It supports:

- reading multiple configured log files
- demo auth log monitoring
- real Linux log monitoring
- auth log parsing
- universal syslog parsing
- raw syslog and kernel log collection
- process snapshot monitoring
- file activity monitoring
- per-file offset state storage
- sending events to the server through HTTP

### mini_siem_server

The server receives events from the agent.

It supports:

- event ingestion
- SQLite storage
- detection rules
- alert generation
- event correlation
- REST API
- SSE realtime updates
- dashboard static file serving

### Web Dashboard

The dashboard displays:

- events
- alerts
- event metadata
- severity
- source
- host
- normalized event details

---

## Final Demonstration Flow

Recommended sequence:

```bash
cd /mnt/mini-siem-usb/mini-siem-usb
./scripts/build.sh
./scripts/run_demo.sh
```

Then:

```text
1. Enter sudo password when requested.
2. Open http://127.0.0.1:6767.
3. Show that the server and agent are running.
4. Show that real Linux logs are collected.
5. Press d in the launcher.
6. Choose logs/bruteforce.txt.
7. Show failed login events in the dashboard.
8. Show generated alerts.
9. Press d again.
10. Choose logs/sudo_test.txt.
11. Show privilege escalation events.
12. Show system_event entries from syslog or kern.log.
13. Press s to open the full server log.
14. Press a to open the full agent log.
15. Press q to stop everything.
```

---

## Purpose

This project demonstrates a compact SIEM-like pipeline:

```text
collection -> normalization -> storage -> detection -> correlation -> dashboard
```

It is intended as an educational Mini-SIEM system for operating system security monitoring.
