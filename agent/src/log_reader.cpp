#include "log_reader.hpp"

#include <filesystem>
#include <fstream>

LogReader::LogReader(const std::string& log_path)
    : log_path_(log_path) {}

const std::string& LogReader::path() const {
    return log_path_;
}

bool LogReader::exists() const {
    return std::filesystem::exists(log_path_) && std::filesystem::is_regular_file(log_path_);
}

std::vector<LogLine> LogReader::read_new_lines(std::uint64_t offset, std::size_t max_lines) {
    std::vector<LogLine> result;

    std::ifstream in(log_path_, std::ios::binary);
    if (!in.is_open()) {
        return result;
    }

    in.seekg(0, std::ios::end);
    const std::streamoff file_size = in.tellg();

    if (file_size < 0) {
        return result;
    }

    std::uint64_t effective_offset = offset;

    // If file was truncated/rotated and offset is beyond EOF, restart from beginning.
    if (effective_offset > static_cast<std::uint64_t>(file_size)) {
        effective_offset = 0;
    }

    in.seekg(static_cast<std::streamoff>(effective_offset), std::ios::beg);

    std::string line;
    while (result.size() < max_lines && std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        std::streamoff pos = in.tellg();
        std::uint64_t next_offset = 0;

        if (pos >= 0) {
            next_offset = static_cast<std::uint64_t>(pos);
        } else {
            // tellg() can be -1 at EOF after successful getline on the last line
            next_offset = static_cast<std::uint64_t>(file_size);
        }

        result.push_back(LogLine{line, next_offset});
    }

    return result;
}
