#include "state_store.hpp"

#include "../../third_party/json.hpp"

#include <filesystem>
#include <fstream>

using json = nlohmann::json;

namespace {

bool read_uint64_json_value(const json& value, std::uint64_t& out) {
    if (value.is_number_unsigned()) {
        out = value.get<std::uint64_t>();
        return true;
    }

    if (value.is_number_integer()) {
        long long v = value.get<long long>();
        if (v < 0) {
            out = 0;
        } else {
            out = static_cast<std::uint64_t>(v);
        }
        return true;
    }

    return false;
}

} // namespace

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

        if (j.contains("files") && j["files"].is_object()) {
            for (auto it = j["files"].begin(); it != j["files"].end(); ++it) {
                std::uint64_t offset = 0;
                if (read_uint64_json_value(it.value(), offset)) {
                    state.file_offsets[it.key()] = offset;
                }
            }
        }

        // Backward compatibility with old state format:
        // {
        //   "log_path": "...",
        //   "offset": 123
        // }
        if (j.contains("log_path") && j["log_path"].is_string() &&
            j.contains("offset")) {
            std::string old_log_path = j["log_path"].get<std::string>();
            std::uint64_t old_offset = 0;

            if (!old_log_path.empty() &&
                read_uint64_json_value(j["offset"], old_offset) &&
                state.file_offsets.find(old_log_path) == state.file_offsets.end()) {
                state.file_offsets[old_log_path] = old_offset;
            }
        }

        if (j.contains("known_processes") && j["known_processes"].is_array()) {
            for (const auto& item : j["known_processes"]) {
                if (!item.is_object()) continue;

                ProcessStateEntry entry;

                if (item.contains("pid") && item["pid"].is_number_integer()) {
                    entry.pid = item["pid"].get<int>();
                } else {
                    continue;
                }

                if (item.contains("process_name") && item["process_name"].is_string()) {
                    entry.process_name = item["process_name"].get<std::string>();
                }

                state.known_processes.push_back(std::move(entry));
            }
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

        j["files"] = json::object();
        for (const auto& pair : state.file_offsets) {
            j["files"][pair.first] = pair.second;
        }

        j["known_processes"] = json::array();
        for (const auto& proc : state.known_processes) {
            j["known_processes"].push_back({
                {"pid", proc.pid},
                {"process_name", proc.process_name}
            });
        }

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
