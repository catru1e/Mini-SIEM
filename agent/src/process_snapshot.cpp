#include "process_snapshot.hpp"

#include <filesystem>
#include <fstream>
#include <cctype>

namespace fs = std::filesystem;

static bool is_pid_dir_name(const std::string& name) {
  if (name.empty()) return false;
  for (char c : name) {
    if (!std::isdigit(static_cast<unsigned char>(c))) {
      return false;
    }
  }
  return true;
}

static std::string read_first_line(const fs::path& path) {
  std::ifstream in(path);
  if (!in.is_open()) return "";

  std::string line;
  std::getline(in, line);
  return line;
}

static std::string read_cmdline(const fs::path& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) return "";

  std::string content((std::istreambuf_iterator<char>(in)),
		       std::istreambuf_iterator<char>());

  std::string cleaned;
  cleaned.reserve(content.size());

  for (char& c : content) {
    if (c == '\0') {
      cleaned.push_back(' ');
    } else if (std::isprint(static_cast<unsigned char>(c))) {
      cleaned.push_back(c);
    }
  }

  while (!content.empty() && content.back() == ' ') {
    content.pop_back();
  }

  return cleaned;
}

std::vector<ProcessInfo> ProcessSnapshot::capture() const {
  std::vector<ProcessInfo> result;

  const fs::path proc_path("/proc");
  if (!fs::exists(proc_path) || !fs::is_directory(proc_path)) {
    return result;
  }

  for (const auto& entry : fs::directory_iterator(proc_path)) {
    if (!entry.is_directory()) continue;

    const std::string name = entry.path().filename().string();
    if(!is_pid_dir_name(name)) continue;

    ProcessInfo info;
    info.pid = std::stoi(name);
    info.process_name = read_first_line(entry.path() / "comm");
    info.cmdline = read_cmdline(entry.path() / "cmdline");

    if (info.process_name.empty()) {
      info.process_name = "unknown";
    }

    result.push_back(std::move(info));
  }

  return result;
}
