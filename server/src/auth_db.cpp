#include "auth_db.hpp"

#include <sqlite3.h>
#include <sodium.h>

#include <chrono>
#include <ctime>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <utility>

static void throw_auth_sqlite(int rc, sqlite3* db, const char* where) {
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
    std::ostringstream oss;
    oss << where << " failed: rc=" << rc;
    if (db) oss << " err=" << sqlite3_errmsg(db);
    throw std::runtime_error(oss.str());
  }
}

static bool column_exists_auth(sqlite3* db, const std::string& table, const std::string& column) {
  std::string sql = "PRAGMA table_info(" + table + ");";

  sqlite3_stmt* stmt = nullptr;
  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    return false;
  }

  bool found = false;

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    const unsigned char* value = sqlite3_column_text(stmt, 1);
    std::string name = value ? reinterpret_cast<const char*>(value) : "";

    if (name == column) {
      found = true;
      break;
    }
  }

  sqlite3_finalize(stmt);
  return found;
}

static void add_integer_column_if_missing_auth(sqlite3* db,
                                               const std::string& table,
                                               const std::string& column,
                                               const std::string& def) {
  if (column_exists_auth(db, table, column)) {
    return;
  }

  std::string sql = "ALTER TABLE " + table + " ADD COLUMN " + column +
                    " INTEGER NOT NULL DEFAULT " + def + ";";

  char* errmsg = nullptr;
  int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errmsg);

  if (rc != SQLITE_OK) {
    std::string err = errmsg ? errmsg : "unknown";
    sqlite3_free(errmsg);
    throw std::runtime_error("sqlite3_exec alter auth failed: " + err);
  }
}

static std::string sqlite_text_auth(sqlite3_stmt* stmt, int col) {
  const unsigned char* value = sqlite3_column_text(stmt, col);
  if (!value) return "";
  return reinterpret_cast<const char*>(value);
}

static std::string now_iso_utc_auth() {
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

static bool valid_role(const std::string& role) {
  return role == "admin" || role == "analyst" || role == "viewer";
}

AuthDb::AuthDb(std::string db_path)
  : db_path_(std::move(db_path)) {
  if (sodium_init() < 0) {
    throw std::runtime_error("sodium_init failed");
  }
}

void AuthDb::init() {
  std::filesystem::path p(db_path_);
  if (p.has_parent_path()) {
    std::filesystem::create_directories(p.parent_path());
  }

  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_auth_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql = R"SQL(
    PRAGMA journal_mode=WAL;
    PRAGMA synchronous=NORMAL;

    CREATE TABLE IF NOT EXISTS users (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      username TEXT NOT NULL UNIQUE,
      password_hash TEXT NOT NULL,
      role TEXT NOT NULL DEFAULT 'viewer',
      enabled INTEGER NOT NULL DEFAULT 1,
      password_change_required INTEGER NOT NULL DEFAULT 0,
      created_at TEXT NOT NULL,
      updated_at TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS sessions (
      id TEXT PRIMARY KEY,
      username TEXT NOT NULL,
      role TEXT NOT NULL,
      created_at TEXT NOT NULL,
      expires_at TEXT NOT NULL
    );

    CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);
    CREATE INDEX IF NOT EXISTS idx_sessions_username ON sessions(username);
    CREATE INDEX IF NOT EXISTS idx_sessions_expires_at ON sessions(expires_at);
  )SQL";

  char* errmsg = nullptr;
  rc = sqlite3_exec(db, sql, nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    std::string err = errmsg ? errmsg : "unknown";
    sqlite3_free(errmsg);
    sqlite3_close(db);
    throw std::runtime_error("sqlite3_exec auth init failed: " + err);
  }
  add_integer_column_if_missing_auth(db, "users", "password_change_required", "0");

  sqlite3_close(db);
}

std::string AuthDb::hash_password(const std::string& password) {
  char out[crypto_pwhash_STRBYTES];

  if (crypto_pwhash_str(out,
                        password.c_str(),
                        password.size(),
                        crypto_pwhash_OPSLIMIT_INTERACTIVE,
                        crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
    throw std::runtime_error("password hashing failed");
  }

  return std::string(out);
}

bool AuthDb::verify_password(const std::string& password, const std::string& hash) {
  return crypto_pwhash_str_verify(hash.c_str(),
                                  password.c_str(),
                                  password.size()) == 0;
}

bool AuthDb::has_users() {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_auth_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql = "SELECT COUNT(*) FROM users;";
  sqlite3_stmt* stmt = nullptr;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_auth_sqlite(rc, db, "sqlite3_prepare_v2");

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_auth_sqlite(rc, db, "sqlite3_step(has_users)");
  }

  const long long count = sqlite3_column_int64(stmt, 0);

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return count > 0;
}

bool AuthDb::create_user(const std::string& username,
                         const std::string& password,
                         const std::string& role,
                         bool enabled,
                         bool password_change_required,
                         std::string& err) {
  try {
    if (username.empty()) {
      err = "username is required";
      return false;
    }

    if (password.empty()) {
      err = "password is required";
      return false;
    }

    if (!valid_role(role)) {
      err = "invalid role";
      return false;
    }

    sqlite3* db = nullptr;
    int rc = sqlite3_open(db_path_.c_str(), &db);
    throw_auth_sqlite(rc, db, "sqlite3_open");

    sqlite3_busy_timeout(db, 5000);

    const std::string now = now_iso_utc_auth();
    const std::string password_hash = hash_password(password);

    const char* sql =
      "INSERT INTO users(username,password_hash,role,enabled,password_change_required,created_at,updated_at) "
      "VALUES(?,?,?,?,?,?,?);";

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    throw_auth_sqlite(rc, db, "sqlite3_prepare_v2");

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, enabled ? 1 : 0);
    sqlite3_bind_int(stmt, 5, password_change_required ? 1 : 0);
    sqlite3_bind_text(stmt, 6, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, now.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (rc != SQLITE_DONE) {
      err = "failed to create user";
      return false;
    }

    return true;
  } catch (const std::exception& e) {
    err = e.what();
    return false;
  }
}

bool AuthDb::verify_user(const std::string& username,
                         const std::string& password,
                         AuthUser& user,
                         std::string& err) {
  try {
    sqlite3* db = nullptr;
    int rc = sqlite3_open(db_path_.c_str(), &db);
    throw_auth_sqlite(rc, db, "sqlite3_open");

    sqlite3_busy_timeout(db, 5000);

    const char* sql =
      "SELECT id, username, password_hash, role, enabled, password_change_required, created_at, updated_at "
      "FROM users WHERE username = ? LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    throw_auth_sqlite(rc, db, "sqlite3_prepare_v2");

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      err = "invalid username or password";
      return false;
    }

    AuthUser found;
    found.id = sqlite3_column_int64(stmt, 0);
    found.username = sqlite_text_auth(stmt, 1);
    const std::string password_hash = sqlite_text_auth(stmt, 2);
    found.role = sqlite_text_auth(stmt, 3);
    found.enabled = sqlite3_column_int(stmt, 4) != 0;
    found.password_change_required = sqlite3_column_int(stmt, 5) != 0;
    found.created_at = sqlite_text_auth(stmt, 6);
    found.updated_at = sqlite_text_auth(stmt, 7);

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (!found.enabled) {
      err = "user is disabled";
      return false;
    }

    if (!verify_password(password, password_hash)) {
      err = "invalid username or password";
      return false;
    }

    user = found;
    return true;
  } catch (const std::exception& e) {
    err = e.what();
    return false;
  }
}

std::vector<AuthUser> AuthDb::get_users() {
  sqlite3* db = nullptr;
  int rc = sqlite3_open(db_path_.c_str(), &db);
  throw_auth_sqlite(rc, db, "sqlite3_open");

  sqlite3_busy_timeout(db, 5000);

  const char* sql =
    "SELECT id, username, role, enabled, password_change_required, created_at, updated_at "
    "FROM users ORDER BY id ASC;";

  sqlite3_stmt* stmt = nullptr;
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
  throw_auth_sqlite(rc, db, "sqlite3_prepare_v2");

  std::vector<AuthUser> users;

  while (true) {
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
      AuthUser u;
      u.id = sqlite3_column_int64(stmt, 0);
      u.username = sqlite_text_auth(stmt, 1);
      u.role = sqlite_text_auth(stmt, 2);
      u.enabled = sqlite3_column_int(stmt, 3) != 0;
      u.password_change_required = sqlite3_column_int(stmt, 4) != 0;
      u.created_at = sqlite_text_auth(stmt, 5);
      u.updated_at = sqlite_text_auth(stmt, 6);
      users.push_back(std::move(u));
      continue;
    }

    if (rc == SQLITE_DONE) break;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    throw_auth_sqlite(rc, db, "sqlite3_step(get_users)");
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return users;
}

bool AuthDb::update_user(const std::string& username,
                         const std::string& role,
                         bool enabled,
                         std::string& err) {
  try {
    if (!valid_role(role)) {
      err = "invalid role";
      return false;
    }

    sqlite3* db = nullptr;
    int rc = sqlite3_open(db_path_.c_str(), &db);
    throw_auth_sqlite(rc, db, "sqlite3_open");

    sqlite3_busy_timeout(db, 5000);

    const std::string now = now_iso_utc_auth();

    const char* sql =
      "UPDATE users SET role = ?, enabled = ?, updated_at = ? "
      "WHERE username = ?;";

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    throw_auth_sqlite(rc, db, "sqlite3_prepare_v2");

    sqlite3_bind_text(stmt, 1, role.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, enabled ? 1 : 0);
    sqlite3_bind_text(stmt, 3, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, username.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (rc != SQLITE_DONE) {
      err = "failed to update user";
      return false;
    }

    return true;
  } catch (const std::exception& e) {
    err = e.what();
    return false;
  }
}

bool AuthDb::delete_user(const std::string& username, std::string& err) {
  try {
    sqlite3* db = nullptr;
    int rc = sqlite3_open(db_path_.c_str(), &db);
    throw_auth_sqlite(rc, db, "sqlite3_open");

    sqlite3_busy_timeout(db, 5000);

    sqlite3_stmt* stmt = nullptr;

    rc = sqlite3_prepare_v2(db, "DELETE FROM sessions WHERE username = ?;", -1, &stmt, nullptr);
    throw_auth_sqlite(rc, db, "sqlite3_prepare_v2(delete sessions)");
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    rc = sqlite3_prepare_v2(db, "DELETE FROM users WHERE username = ?;", -1, &stmt, nullptr);
    throw_auth_sqlite(rc, db, "sqlite3_prepare_v2(delete user)");
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_close(db);

    if (rc != SQLITE_DONE) {
      err = "failed to delete user";
      return false;
    }

    return true;
  } catch (const std::exception& e) {
    err = e.what();
    return false;
  }
}

bool AuthDb::update_password(const std::string& username,
                             const std::string& password,
                             bool password_change_required,
                             std::string& err) {
  try {
    if (password.empty()) {
      err = "password is required";
      return false;
    }

    sqlite3* db = nullptr;
    int rc = sqlite3_open(db_path_.c_str(), &db);
    throw_auth_sqlite(rc, db, "sqlite3_open");

    sqlite3_busy_timeout(db, 5000);

    const std::string now = now_iso_utc_auth();
    const std::string password_hash = hash_password(password);

    const char* sql =
      "UPDATE users SET password_hash = ?, password_change_required = ?, updated_at = ? "
      "WHERE username = ?;";

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    throw_auth_sqlite(rc, db, "sqlite3_prepare_v2");

    sqlite3_bind_text(stmt, 1, password_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, password_change_required ? 1 : 0);
    sqlite3_bind_text(stmt, 3, now.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, username.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (rc != SQLITE_DONE) {
      err = "failed to update password";
      return false;
    }

    return true;
  } catch (const std::exception& e) {
    err = e.what();
    return false;
  }
}

bool AuthDb::change_password(const std::string& username,
                             const std::string& old_password,
                             const std::string& new_password,
                             std::string& err) {
  if (new_password.empty()) {
    err = "new password is required";
    return false;
  }

  if (old_password == new_password) {
    err = "new password must be different";
    return false;
  }

  AuthUser user;
  if (!verify_user(username, old_password, user, err)) {
    return false;
  }

  return update_password(username, new_password, false, err);
}
