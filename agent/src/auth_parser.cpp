#include "auth_parser.hpp"

#include <regex>

using json = nlohmann::json;

std::string AuthParser::extract_process_name(const std::string& line) const {
	// example
	// Mar 16 12:00:00 localhost sshd[123]: Failed password ...
	static const std::regex proc_re(R"(^[A-Z][a-z]{2}\s+\d+\s+\d{2}:\d{2}:\d{2}\s+\S+\s+([a-zA-Z0-9_\-./]+)(?:\[\d+\])?:)");

	std::smatch m;
	if (std::regex_search(line, m, proc_re) && m.size() > 1) {
		return m[1].str();
	}
	return "unknown";
}

std::optional<ParsedAuthEvent> AuthParser::parse(const std::string& line) const {
	ParsedAuthEvent out;
	out.fields["raw"] = line;
	out.fields["process"] = extract_process_name(line);

	std::smatch m;

	//1) Failed password
	//Failed password for root from 1.2.3.4 port 51234 ssh2
	//Failed password for invalid user admin from 1.2.3.4 port 51234 ssh2
	static const std::regex failed_re(
		R"(Failed password for (?:invalid user )?(\S+) from ([0-9]{1,3}(?:\.[0-9]{1,3}){3}) port (\d+))",
		std::regex::icase
	);

	if (std::regex_search(line, m, failed_re) && m.size() >= 4) {
		out.event_type = "auth_failed";
		out.severity = "medium";
		out.fields["user"] = m[1].str();
		out.fields["src_ip"] = m[2].str();
		out.fields["port"] = m[3].str();
		return out;
	}

	//2) Accepted password
	//Accepted password for admin from 5.6.7.8 port 42311 ssh2
    	static const std::regex accepted_re(
        	R"(Accepted password for (\S+) from ([0-9]{1,3}(?:\.[0-9]{1,3}){3}) port (\d+))",
        	std::regex::icase
    	);

    	if (std::regex_search(line, m, accepted_re) && m.size() >= 4) {
        	out.event_type = "auth_success";
        	out.severity = "info";
       	 	out.fields["user"] = m[1].str();
        	out.fields["src_ip"] = m[2].str();
        	out.fields["port"] = m[3].str();
        	return out;
    	}

	//3) Invalid user
	//Invalid user test from 1.2.3.4 port 3333
	static const std::regex invalid_user_re(
		R"(Invalid user (\S+) from ([0-9]{1,3}(?:\.[0-9]{1,3}){3})(?: port (\d+))?)",
		std::regex::icase
	);

	if (std::regex_search(line, m, invalid_user_re) && m.size() >= 3){
		out.event_type = "auth_invalid_user";
		out.severity = "medium";
		out.fields["user"] = m[1].str();
		out.fields["src_ip"] = m[2].str();
		if (m.size() >= 4 && m[3].matched) {
			out.fields["port"] = m[3].str();
		}
		return out;
	}

	//4) sudo usage
	//sudo: aram : TTY=pts/0 ; PWD=/home/aram ; USER=root ; COMMAND=/usr/bin/cat /etc/shadow
	static const std::regex sudo_re(
		R"(sudo:\s+(\S+)\s*:.*COMMAND=(.+)$)",
		std::regex::icase
	);

	if (std::regex_search(line, m, sudo_re) && m.size() >= 3) {
		out.event_type = "privilege_escalation";
		out.severity = "high";
		out.fields["user"] = m[1].str();
		out.fields["command"] = m[2].str();
		return out;
	}

	//5) session opened
	//pam_unix(sshd:session): session opened for user root by (uid=0)
	static const std::regex session_opened_re(
		R"(session opened for user (\S+))",
		std::regex::icase
	);

	if (std::regex_search(line, m, session_opened_re) && m.size() >= 2) {
		out.event_type = "session_open";
		out.severity = "info";
		out.fields["user"] = m[1].str();
		return out;
	}

	//6) session closed
	//pam_unix(sshd::session): session closed for user root
	static const std::regex session_closed_re(
		R"(session closed for user (\S+))",
		std::regex::icase
	);

	if (std::regex_search(line, m, session_closed_re) && m.size() >= 2) {
		out.event_type = "session_close";
		out.severity = "info";
		out.fields["user"] = m[1].str();
		return out;
	}

	return std::nullopt;
}

















