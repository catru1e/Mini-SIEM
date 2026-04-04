#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct ProcessStateEntry {
    int pid = 0;
    std::string process_name;
};

struct AgentState {
    std::string log_path;
    std::uint64_t offset = 0;
    std::vector<ProcessStateEntry> known_processes;
};

class StateStore {
public:
    explicit StateStore(const std::string& state_file_path);

    AgentState load() const;
    bool save(const AgentState& state) const;

private:
    std::string state_file_path_;
};
