#pragma once

#include <string>
#include <vector>

struct DbEventRow {
  long long id = 0;
  std::string ts;
  std::string event_type;
  std::string source;
  std::string json;
};

class SqliteDb {
public:
  explicit SqliteDb(std::string db_path);

  // dealem tablicu esli netu
  void init();

  // zapishem shtuchki
  void insert_event(const std::string& ts,
                    const std::string& event_type,
                    const std::string& source,
                    const std::string& json);

  //prochitat poslednie N shtuki (id DESC)
  std::vector<DbEventRow> get_last_events(int limit);

private:
  std::string db_path_;
};
