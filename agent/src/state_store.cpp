#include "state_store.hpp"

#include "../../third_party/json.hpp"

#include <filesystem>
#include <fstream>

using json = nlohmann::json;

StateStore::StateStore(const std::string& state_file_path)
    : state_file_path_(state_file_path) {}

AgentState StateStore::load() const {
    AgentState state;

    if (!std::filesystem::exists(state_file_path_)) {
        return state;
    }

    std::ifstream in(state_file_path_);
    if (!in.is_open()) {
        return state;
    }

    try {
        json j;
        in >> j;

        if (j.contains("log_path") && j["log_path"].is_string()) {
            state.log_path = j["log_path"].get<std::string>();
        }

        if (j.contains("offset") && j["offset"].is_number_unsigned()) {
            state.offset = j["offset"].get<std::uint64_t>();
        } else if (j.contains("offset") && j["offset"].is_number_integer()) {
            auto v = j["offset"].get<long long>();
            state.offset = v >= 0 ? static_cast<std::uint64_t>(v) : 0;
        }
    } catch (...) {
        // Corrupted or invalid state file: return default state.
    }

    return state;
}

bool StateStore::save(const AgentState& state) const {
    try {
        std::filesystem::path p(state_file_path_);
        if (p.has_parent_path()) {
            std::filesystem::create_directories(p.parent_path());
        }

        json j;
        j["log_path"] = state.log_path;
        j["offset"] = state.offset;

        std::ofstream out(state_file_path_, std::ios::trunc);
        if (!out.is_open()) {
            return false;
        }

        out << j.dump(2) << "\n";
        return true;
    } catch (...) {
        return false;
    }
}
