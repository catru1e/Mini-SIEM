#include "syslog_parser.hpp"

#include <regex>
#include <string>

using json = nlohmann::json;

std::string SyslogParser::detect_program(const std::string& line) {
    // ISO format example:
    // 2026-04-26T15:46:38.730185+00:00 smth kernel: message
    // 2026-04-26T15:46:38.730185+00:00 smth systemd[1]: message
    {
        static const std::regex r(
            R"(^\S+\s+\S+\s+([A-Za-z0-9_.\/:-]+)(?:\[[0-9]+\])?:)"
        );

        std::smatch m;
        if (std::regex_search(line, m, r) && m.size() > 1) {
            std::string program = m[1].str();

            if (program == "kernel") {
                return "kernel";
            }

            return program;
        }
    }

    // Classic syslog format example:
    // Apr 20 22:30:00 smth kernel: message
    // Apr 20 22:30:00 smth systemd[1]: message
    {
        static const std::regex r(
            R"(^[A-Z][a-z]{2}\s+\d{1,2}\s+\d{2}:\d{2}:\d{2}\s+\S+\s+([A-Za-z0-9_.\/:-]+)(?:\[[0-9]+\])?:)"
        );

        std::smatch m;
        if (std::regex_search(line, m, r) && m.size() > 1) {
            std::string program = m[1].str();

            if (program == "kernel") {
                return "kernel";
            }

            return program;
        }
    }

    if (line.find("kernel:") != std::string::npos) {
        return "kernel";
    }

    return "unknown";
}

std::string SyslogParser::detect_message(const std::string& line) {
    std::size_t pos = line.find(": ");

    if (pos == std::string::npos) {
        return line;
    }

    return line.substr(pos + 2);
}

std::string SyslogParser::detect_severity(const std::string& line) {
    std::string lower = line;

    for (char& c : lower) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }

    if (lower.find("panic") != std::string::npos ||
        lower.find("critical") != std::string::npos ||
        lower.find("fatal") != std::string::npos ||
        lower.find("segfault") != std::string::npos) {
        return "high";
    }

    if (lower.find("error") != std::string::npos ||
        lower.find("failed") != std::string::npos ||
        lower.find("failure") != std::string::npos ||
        lower.find("denied") != std::string::npos ||
        lower.find("warning") != std::string::npos ||
        lower.find("warn") != std::string::npos) {
        return "medium";
    }

    return "low";
}

json SyslogParser::parse(const std::string& line,
                         const std::string& source_name) const {
    json event;

    event["source"] = source_name.empty() ? "syslog" : source_name;
    event["event_type"] = "system_event";
    event["program"] = detect_program(line);
    event["message"] = detect_message(line);
    event["severity"] = detect_severity(line);
    event["raw"] = line;

    return event;
}
