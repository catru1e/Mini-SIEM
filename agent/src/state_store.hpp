#pragma once

#include <cstdint>
#include <string>

struct AgentState {
    std::string log_path;
    std::uint64_t offset = 0;
};

class StateStore {
public:
    explicit StateStore(const std::string& state_file_path);

    AgentState load() const;
    bool save(const AgentState& state) const;

private:
    std::string state_file_path_;
};
