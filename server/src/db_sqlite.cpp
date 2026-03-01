#include "db_sqlite.hpp"

#include <sqlite3.h>

#include <stdexcept>
#include <sstream>

static void throw_sqlite(int rc, sqlite3* db, const char* where) {
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
    std::ostringstream oss;
    oss << where << " failed: rc=" << rc;
    if (db) oss << " err=" << sqlite3_errmsg(db);
    throw std::runtime_error(oss.str());
  }
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

  const char* sql = R"SQL(
    PRAGMA journal_mode=WAL;

    CREATE TABLE IF NOT EXISTS events (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      ts TEXT NOT NULL,
      event_type TEXT NOT NULL,
      source TEXT NOT NULL,
      json TEXT NOT NULL
    );

    CREATE INDEX IF NOT EXISTS idx_events_ts ON events(ts);
    CREATE INDEX IF NOT EXISTS idx_events_type ON events(event_type);
  )SQL";

  char* errmsg = nullptr;
  rc = sqlite3_exec(db, sql, nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    std::string err = errmsg ? errmsg : "unknown";
    sqlite3_free(errmsg);
    sqlite3_close(db);
    throw std::runtime_error("sqlite3_exec init failed: " + err);
  }

  sqlite3_close(db);
}

void SqliteDb::insert_event(const std::string& ts,
                            const std::string& event_type,
                            const std::string& source,
                            const std::string& json) {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_sqlite(rc, db, "sqlite3_open");

  const char* sql = "INSERT INTO events(ts,event_type,source,json) VALUES(?,?,?,?);";
  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_sqlite(rc, db, "sqlite3_prepare_v2");

  sqlite3_bind_text(stmt, 1, ts.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, event_type.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, source.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, json.c_str(), -1, SQLITE_TRANSIENT);

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

  const char* sql = "SELECT id, ts, event_type, source, json FROM events ORDER BY id DESC LIMIT ?;";
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
      r.ts = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
      r.event_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
      r.source = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
      r.json = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
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
