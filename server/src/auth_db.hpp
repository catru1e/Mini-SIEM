#pragma once

#include <string>
#include <vector>

struct AuthUser {
  long long id = 0;
  std::string username;
  std::string role;
  bool enabled = true;
  bool password_change_required = false;
  std::string created_at;
  std::string updated_at;
};

class AuthDb {
public:
  explicit AuthDb(std::string db_path);

  void init();

  bool has_users();

  bool create_user(const std::string& username,
                   const std::string& password,
                   const std::string& role,
                   bool enabled,
                   bool password_change_required,
                   std::string& err);

  bool verify_user(const std::string& username,
                   const std::string& password,
                   AuthUser& user,
                   std::string& err);

  std::vector<AuthUser> get_users();

  bool update_user(const std::string& username,
                   const std::string& role,
                   bool enabled,
                   std::string& err);

  bool delete_user(const std::string& username, std::string& err);

  bool update_password(const std::string& username,
                       const std::string& password,
                       bool password_change_required,
                       std::string& err);
  bool change_password(const std::string& username,
                       const std::string& old_password,
                       const std::string& new_password,
                       std::string& err);

private:
  std::string db_path_;

  std::string hash_password(const std::string& password);
  bool verify_password(const std::string& password, const std::string& hash);
};
