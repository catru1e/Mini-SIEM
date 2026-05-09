#include "db_sqlite.hpp"
#include "../../third_party/json.hpp"

#include <sqlite3.h>

#include <iostream>
#include <chrono>
#include <ctime>
#include <stdexcept>
#include <sstream>

using json = nlohmann::json;

static void throw_sqlite(int rc, sqlite3* db, const char* where) {
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
    std::ostringstream oss;
    oss << where << " failed: rc=" << rc;
    if (db) oss << " err=" << sqlite3_errmsg(db);
    throw std::runtime_error(oss.str());
  }
}

static std::string sqlite_text(sqlite3_stmt* stmt, int col) {
  const unsigned char* value = sqlite3_column_text(stmt, col);
  if (!value) return "";
  return reinterpret_cast<const char*>(value);
}

static bool column_exists(sqlite3* db, const std::string& table, const std::string& column) {
  std::string sql = "PRAGMA table_info(" + table + ");";

  sqlite3_stmt* stmt = nullptr;
  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    return false;
  }

  bool found = false;

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    std::string name = sqlite_text(stmt, 1);
    if (name == column) {
      found = true;
      break;
    }
  }

  sqlite3_finalize(stmt);
  return found;
}

static void add_text_column_if_missing(sqlite3* db, const std::string& table, const std::string& column) {
  if (column_exists(db, table, column)) {
    return;
  }

  std::string sql = "ALTER TABLE " + table + " ADD COLUMN " + column + " TEXT;";

  char* errmsg = nullptr;
  int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    std::string err = errmsg ? errmsg : "unknown";
    sqlite3_free(errmsg);
    throw std::runtime_error("sqlite3_exec alter failed: " + err);
  }
}

static std::string now_iso_utc() {
  using namespace std::chrono;

  auto now = system_clock::now();
  auto secs = time_point_cast<std::chrono::seconds>(now);
  auto ms = duration_cast<milliseconds>(now - secs).count();

  std::time_t t = system_clock::to_time_t(now);
  std::tm tm{};
  gmtime_r(&t, &tm);

  char buf[64];
  std::snprintf(buf, sizeof(buf),
		     "%04d-%02d-%02dT%02d:%02d:%02d.%03lldZ",
		     tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		     tm.tm_hour, tm.tm_min, tm.tm_sec,
		     static_cast<long long>(ms));
  return std::string(buf);
}

SqliteDb::SqliteDb(std::string db_path)
  : db_path_(std::move(db_path)) {}

void SqliteDb::init() {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  if (rc != SQLITE_OK) {
    std::string err = db ? sqlite3_errmsg(db) : "unknown";
    if (db) sqlite3_close(db);
    throw std::runtime_error("sqlite3_open failed: " + err);
  }

  sqlite3_busy_timeout(db, 5000);

  const char* sql = R"SQL(
    PRAGMA journal_mode=WAL;
    PRAGMA synchronous=NORMAL;

    CREATE TABLE IF NOT EXISTS events (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      event_id TEXT,
      ts TEXT NOT NULL,
      received_at TEXT NOT NULL,
      host TEXT,
      event_type TEXT NOT NULL,
      source TEXT NOT NULL,
      source_type TEXT,
      severity TEXT,
      json TEXT NOT NULL
    );

    CREATE INDEX IF NOT EXISTS idx_events_event_id ON events(event_id);
    CREATE INDEX IF NOT EXISTS idx_events_ts ON events(ts);
    CREATE INDEX IF NOT EXISTS idx_events_type ON events(event_type);
    CREATE INDEX IF NOT EXISTS idx_events_source ON events(source);
    CREATE INDEX IF NOT EXISTS idx_events_source_type ON events(source_type);
    CREATE INDEX IF NOT EXISTS idx_events_host ON events(host);
    CREATE INDEX IF NOT EXISTS idx_events_severity ON events(severity);

    CREATE TABLE IF NOT EXISTS alerts (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      ts TEXT NOT NULL,
      rule_name TEXT NOT NULL,
      severity TEXT NOT NULL,
      title TEXT NOT NULL,
      description TEXT NOT NULL,
      json TEXT NOT NULL
    );

    CREATE INDEX IF NOT EXISTS idx_alerts_ts ON alerts(ts);
    CREATE INDEX IF NOT EXISTS idx_alerts_rule_name ON alerts(rule_name);
  )SQL";

  char* errmsg = nullptr;
  rc = sqlite3_exec(db, sql, nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    std::string err = errmsg ? errmsg : "unknown";
    sqlite3_free(errmsg);
    sqlite3_close(db);
    throw std::runtime_error("sqlite3_exec init failed: " + err);
  }

  add_text_column_if_missing(db, "events", "event_id");
  add_text_column_if_missing(db, "events", "source_type");

  sqlite3_close(db);
}

void SqliteDb::insert_event(const std::string& event_id,
                            const std::string& ts,
                            const std::string& received_at,
                            const std::string& event_type,
                            const std::string& source,
                            const std::string& source_type,
                            const std::string& json_str) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  std::string host = "unknown";
  std::string severity = "info";

  try {
    auto j = json::parse(json_str);

    if (j.contains("host") && j["host"].is_string()) {
      host = j["host"].get<std::string>();
    }
    if (j.contains("severity") && j["severity"].is_string()){
      severity = j["severity"].get<std::string>();
    }
  } catch (...) {
    //keep defaults if JSON parsing fails
  }

  const char* sql =
    "INSERT INTO events(event_id,ts,received_at,host,event_type,source,source_type,severity,json) "
    "VALUES(?,?,?,?,?,?,?,?,?);";

  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, event_id.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, received_at.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, host.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, event_type.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 6, source.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 7, source_type.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 8, severity.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 9, json_str.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  throw_sqlite(rc, db, "sqlite3_step(insert)");

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

std::vector<DbEventRow> SqliteDb::get_last_events(int limit) {
  if (limit <= 0) limit = 100;
  if (limit > 1000) limit = 1000;

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
	"SELECT id, event_id, ts, received_at, host, event_type, source, source_type, severity, json FROM events ORDER BY id DESC LIMIT ?;";
  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_int(stmt, 1, limit);

  std::vector<DbEventRow> rows;
  while (true) {
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
      DbEventRow r;
      r.id = sqlite3_column_int64(stmt, 0);
      r.event_id = sqlite_text(stmt, 1);
      r.ts = sqlite_text(stmt, 2);
      r.received_at = sqlite_text(stmt, 3);
      r.host = sqlite_text(stmt, 4);
      r.event_type = sqlite_text(stmt, 5);
      r.source = sqlite_text(stmt, 6);
      r.source_type = sqlite_text(stmt, 7);
      r.severity = sqlite_text(stmt, 8);
      r.json = sqlite_text(stmt, 9);
      rows.push_back(std::move(r));
      continue;
    }
    if (rc == SQLITE_DONE) break;
    throw_sqlite(rc, db, "sqlite3_step(select)");
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return rows;
}


// !!!!!!!!!!!!!!!!! ALERT PART !!!!!!!!!!!!!!!!!!!
long long SqliteDb::count_auth_failed_by_src_ip_since(const std::string& src_ip, const std::string& since_ts) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = 'auth_failed' "
    "AND ts >= ? "
    "AND json_extract(json, '$.src_ip') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, src_ip.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_auth_failed_by_src_ip_since)");
  }

  long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count;
}

long long SqliteDb::count_auth_failed_by_user_since(const std::string& user, const std::string& since_ts) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = 'auth_failed' "
    "AND ts >= ? "
    "AND json_extract(json, '$.user') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_auth_failed_by_user_since)");
  }

  long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count;
}

long long SqliteDb::count_auth_invalid_user_by_src_ip_since(const std::string& src_ip, const std::string& since_ts) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = 'auth_invalid_user' "
    "AND ts >= ? "
    "AND json_extract(json, '$.src_ip') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, src_ip.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_auth_invalid_user_by_src_ip_since)");
  }

  long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count;
}

long long SqliteDb::count_privilege_escalation_by_user_since(const std::string& user, const std::string& since_ts) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = 'privilege_escalation' "
    "AND ts >= ? "
    "AND json_extract(json, '$.user') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_privilege_escalation_by_user_since)");
  }

  long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count;
}

long long SqliteDb::count_auth_success_by_user_since(const std::string& user, const std::string& since_ts) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM events "
    "WHERE event_type = 'auth_success' "
    "AND ts >= ? "
    "AND json_extract(json, '$.user') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, user.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(count_auth_success_by_user_since)");
  }

  long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count;
}

bool SqliteDb::has_recent_alert_for_rule_and_user_since(
    const std::string& rule_name,
    const std::string& user,
    const std::string& since_ts) {

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT COUNT(*) "
    "FROM alerts "
    "WHERE rule_name = ? "
    "AND ts >= ? "
    "AND json_extract(json, '$.user') = ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, rule_name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, since_ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, user.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_sqlite(rc, db, "sqlite3_step(has_recent_alert_for_rule_and_user_since)");
  }

  const long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return count > 0;
}

void SqliteDb::insert_alert(const std::string& ts,
			    const std::string& rule_name,
			    const std::string& severity,
			    const std::string& title,
			    const std::string& description,
			    const std::string& json_str) {
  std::cerr << "[db] insert_alert rule_name=" << rule_name
          << " severity=" << severity
          << " title=" << title << "\n";

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql = "INSERT INTO alerts(ts, rule_name, severity, title, description, json) "
		    "VALUES(?,?,?,?,?,?);";
  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, rule_name.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, severity.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, title.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, description.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 6, json_str.c_str(), -1, SQLITE_TRANSIENT);

  rc = sqlite3_step(stmt);
  throw_sqlite(rc, db, "sqlite3_step(insert_alert)");

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}


std::vector<DbAlertRow> SqliteDb::get_last_alerts(int limit) {
  if (limit <= 0) limit = 100;
  if (limit > 1000) limit = 1000;

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
      "SELECT id, ts, rule_name, severity, title, description, json "
      "FROM alerts "
      "ORDER BY id DESC "
      "LIMIT ?;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_int(stmt, 1, limit);

  std::vector<DbAlertRow> rows;
  while (true) {
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
      DbAlertRow r;
      r.id = sqlite3_column_int64(stmt, 0);
      r.ts = sqlite_text(stmt, 1);
      r.rule_name = sqlite_text(stmt, 2);
      r.severity = sqlite_text(stmt, 3);
      r.title = sqlite_text(stmt, 4);
      r.description = sqlite_text(stmt, 5);
      r.json = sqlite_text(stmt, 6);
      rows.push_back(std::move(r));
      continue;
    }
    if (rc == SQLITE_DONE) break;
    throw_sqlite(rc, db, "sqlite3_step(get_last_alerts)");
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return rows;
}

std::vector<std::string> SqliteDb::get_recent_privilege_escalation_commands_by_user_since(
    const std::string& user,
    const std::string& since_ts,
    int limit) {

  std::vector<std::string> commands;

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  if (rc != SQLITE_OK) {
    return commands;
  }

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
      "SELECT json FROM events "
      "WHERE event_type = 'privilege_escalation' AND ts >= ? "
      "ORDER BY ts DESC;";

  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    return commands;
  }

  sqlite3_bind_text(stmt, 1, since_ts.c_str(), -1, SQLITE_TRANSIENT);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char* json_text = sqlite3_column_text(stmt, 0);
    if (!json_text) continue;

    try {
      auto j = nlohmann::json::parse(reinterpret_cast<const char*>(json_text));

      if (!j.contains("user") || !j["user"].is_string()) continue;
      if (j["user"].get<std::string>() != user) continue;

      if (!j.contains("command") || !j["command"].is_string()) continue;

      const std::string cmd = j["command"].get<std::string>();

      // убрать дубликаты
      bool exists = false;
      for (const auto& existing : commands) {
        if (existing == cmd) {
          exists = true;
          break;
        }
      }

      if (!exists) {
        commands.push_back(cmd);
      }

      if (static_cast<int>(commands.size()) >= limit) break;

    } catch (...) {
      continue;
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return commands;
}
