# Mini-SIEM USB

Mini-SIEM USB is a simplified Security Information and Event Management system for Linux security monitoring.

The system collects operating system security events, normalizes them, stores them in SQLite, applies detection and correlation logic, and displays events and alerts in a web dashboard.

The project is designed to run as a portable directory from a USB drive.

---

## Features

- Linux security log collection
- Authentication log parsing
- Failed login detection
- Successful login detection
- Sudo / privilege escalation detection
- Process monitoring through `/proc`
- File activity monitoring
- SQLite event storage
- Detection alerts
- Event correlation
- Web dashboard
- Realtime dashboard updates
- Portable USB demo launcher

---

## Architecture

```text
Linux logs / system sources
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
├── agent/          # event collection, log parsing, process/file monitoring
├── server/         # HTTP API, SQLite storage, detection, correlation
├── common/         # shared configuration loader
├── config/         # runtime configuration
├── data/           # SQLite database and agent state
├── dist/           # compiled binaries
├── logs/           # runtime logs and demo payloads
├── scripts/        # build, run, demo and reset scripts
├── web/            # dashboard frontend
├── README.md       # project overview
└── RUN.md          # detailed USB run guide
```

---

## Quick Start

From the project root:

```bash
./scripts/reset_demo.sh
./scripts/build.sh
./scripts/run_demo.sh
```

Then open the dashboard:

```text
http://127.0.0.1:6767
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

## USB Execution Note

If the USB drive is mounted with `noexec`, Linux will not allow scripts or binaries to run directly from the USB drive.

Typical error:

```text
Permission denied
```

In this case, remount the USB drive with the `exec` option.

Detailed instructions are available in:

```text
RUN.md
```

---

## Detailed Run Guide

See [RUN.md](RUN.md) for:

- USB mount instructions
- build and run steps
- demo launcher usage
- demo payload examples
- API checks
- troubleshooting
- final demonstration flow

---

## Main Components

### mini_siem_agent

The agent collects security-related events from the operating system.

It supports:

- auth log reading
- log normalization
- process snapshot monitoring
- file activity monitoring
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
./scripts/reset_demo.sh
./scripts/build.sh
./scripts/run_demo.sh
```

Then:

```text
1. Open http://127.0.0.1:6767
2. Press d in the launcher
3. Choose logs/bruteforce.txt
4. Show events in the dashboard
5. Show generated alerts
6. Press d again
7. Choose logs/sudo_test.txt
8. Show privilege escalation events
9. Press s to open the full server log
10. Press a to open the full agent log
11. Press q to stop everything
```

---

## Purpose

This project demonstrates a compact SIEM-like pipeline:

```text
collection -> normalization -> storage -> detection -> correlation -> dashboard
```

It is intended as an educational Mini-SIEM system for operating system security monitoring.
