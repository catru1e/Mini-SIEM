#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct FileEvent {
  std::string event_type;
  std::string watched_path;
  std::string relative_path;
  std::string full_path;
  std::uint32_t mask = 0;
};

class FileMonitor {
public:
  FileMonitor();
  ~FileMonitor();

  bool init(const std::string& path);
  std::vector<FileEvent> poll_events();

  const std::string& watched_path() const;

private:
  int fd_ = -1;
  int wd_ = -1;
  std::string watched_path_;
};
