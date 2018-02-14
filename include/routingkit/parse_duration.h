#ifndef ROUTING_KIT_PARSE_DURATION_H
#define ROUTING_KIT_PARSE_DURATION_H
#include <string>
#include <regex>

namespace RoutingKit{
unsigned match_duration(const std::string& input, const std::regex& re, unsigned default_value) {
	std::smatch match;
	std::regex_search(input, match, re);
	if (match.empty()) {
		return default_value;
	}
	std::vector<double> vec = {0,0,0,0,0,0};
	for (size_t i = 1; i < match.size(); ++i) {
		if (match[i].matched) {
			std::string str = match[i];
			str.pop_back(); // remove last character.
			vec[i-1] = std::stod(str);
		}
	}
	unsigned duration =
		31556926   * vec[0] +  // years
		2629744    * vec[1] +  // months
		86400      * vec[2] +  // days
		3600       * vec[3] +  // hours
		60         * vec[4] +  // minutes
		1          * vec[5];   // seconds
	if (duration == 0) {
		return default_value;
	}
	return duration;
}

unsigned parse_duration(const char* duration) {
	// Simple mm, hh:mm hh:mm:ss duration.
	static const std::regex simple_a("(\\d?\\d)");
	static const std::regex simple_b("(\\d?\\d):(\\d\\d)");
	static const std::regex simple_c("(\\d?\\d):(\\d\\d):(\\d\\d)");
	// ISO 8601 duration parsing (by https://github.com/sigidagi).
	static const std::regex has_time_component("^((?!T).)*$");
	static const std::regex without_time("P([[:d:]]+Y)?([[:d:]]+M)?([[:d:]]+D)?");
	static const std::regex with_time("P([[:d:]]+Y)?([[:d:]]+M)?([[:d:]]+D)?T([[:d:]]+H)?([[:d:]]+M)?([[:d:]]+S|[[:d:]]+\\.[[:d:]]+S)?");

  std::cmatch match;
	unsigned result = 600; // Default 10 minutes.
	if (std::regex_match(duration, match, simple_a)) {
		result = std::stoi(match[1].str()) * 60;
	} else if (std::regex_match(duration, match, simple_b)) {
		result = std::stoi(match[1].str()) * 60 * 60 + stoi(match[2].str()) * 60;
	} else if (std::regex_match(duration, match, simple_c)) {
		result = std::stoi(match[1].str()) * 60 * 60 + stoi(match[2].str()) * 60 + stoi(match[3]);
	} else {
			if (std::regex_match(duration, has_time_component))
					result = match_duration(duration, without_time, result);
			else
					result = match_duration(duration, with_time, result);
	}
	return result;
}
} // RoutingKit

#endif
