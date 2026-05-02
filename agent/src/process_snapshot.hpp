#pragma once

#include <string>
#include <vector>

struct ProcessInfo {
  int pid = 0;
  std::string process_name;
  std::string cmdline;
};

class ProcessSnapshot {
public:
  std::vector<ProcessInfo> capture() const;
};
