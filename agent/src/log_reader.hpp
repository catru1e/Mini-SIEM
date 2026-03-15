#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct LogLine {
    std::string text;
    std::uint64_t next_offset = 0;
};

class LogReader {
public:
    explicit LogReader(const std::string& log_path);

    const std::string& path() const;
    bool exists() const;

    std::vector<LogLine> read_new_lines(std::uint64_t offset, std::size_t max_lines = 100);

private:
    std::string log_path_;
};
