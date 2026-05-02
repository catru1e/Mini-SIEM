#pragma once

#include "json.hpp"

#include <optional>
#include <string>

struct  ParsedAuthEvent{
	std::string event_type;
	std::string severity;
	nlohmann::json fields;
};

class AuthParser {
public:
	std::optional<ParsedAuthEvent> parse(const std::string& line) const;

private:
	std::string extract_process_name(const std::string& line) const;
};
