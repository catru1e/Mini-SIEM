#pragma once

#include <string>
#include <vector>
#include <map>

struct DbEventRow {
  long long id = 0;
  std::string event_id;
  std::string ts;
  std::string received_at;
  std::string host;
  std::string event_type;
  std::string source;
  std::string source_type;
  std::string severity;
  std::string json;
};

struct DbAlertRow {
  long long id = 0;
  std::string ts;
  std::string rule_name;
  std::string severity;
  std::string title;
  std::string description;
  std::string json;
};

struct DbTimeBucket {
  std::string bucket;
  long long count = 0;
};

class SqliteDb {
public:
  explicit SqliteDb(std::string db_path);

  // delaem tablicu esli netu
  void init();

  // zapishem shtuchki
  void insert_event(const std::string& event_id,
                    const std::string& ts,
                    const std::string& received_at,
                    const std::string& event_type,
                    const std::string& source,
                    const std::string& source_type,
                    const std::string& json);

  //prochitat poslednie N shtuki (id DESC)
  std::vector<DbEventRow> get_last_events(int limit);
  std::vector<DbAlertRow> get_last_alerts(int limit); //ATTENTION IT IS ALERTS

  long long count_events_by_field_since(const std::string& event_type,
                                        const std::string& field_name,
                                        const std::string& field_value,
                                        const std::string& since_ts);

  bool has_recent_alert_for_rule_and_group_since(const std::string& rule_id,
                                                 const std::string& group_key,
                                                 const std::string& since_ts);

  std::vector<std::string> get_recent_privilege_escalation_commands_by_user_since(
    const std::string& user,
    const std::string& since_ts,
    int limit);

  long long count_auth_failed_by_src_ip_since(const std::string& src_ip, const std::string& since_ts);
  long long count_auth_failed_by_user_since(const std::string& user, const std::string& since_ts);
  long long count_auth_invalid_user_by_src_ip_since(const std::string& src_ip, const std::string& since_ts);
  long long count_privilege_escalation_by_user_since(const std::string& user, const std::string& since_ts);
  long long count_auth_success_by_user_since(const std::string& user, const std::string& since_ts);

//20.
  long long count_all_events();
  long long count_all_alerts();

  std::map<std::string, long long> count_events_by_event_type();
  std::map<std::string, long long> count_events_by_source_type();
  std::map<std::string, long long> count_alerts_by_severity();

  std::vector<DbTimeBucket> count_events_over_time(int limit);
//end 20.

  bool has_recent_alert_for_rule_and_user_since(
    const std::string& rule_name,
    const std::string& user,
    const std::string& since_ts);

  void insert_alert(const std::string& ts,
                    const std::string& rule_name,
                    const std::string& severity,
                    const std::string& title,
                    const std::string& description,
                    const std::string& json);
private:
  std::string db_path_;
};
