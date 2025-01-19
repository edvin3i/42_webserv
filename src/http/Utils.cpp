#include "../../includes/http/Utils.hpp"

const std::string Utils::whitespace = " \t";

bool Utils::is_whitespace(char c)
{
	return (whitespace.find(c) != std::string::npos);
}

std::vector<std::string> Utils::split(const std::string& str, const std::string& delimiter)
{
	std::vector<std::string> tokens;
	size_t last = 0, next = 0;
	std::string token;
	while ((next = str.find(delimiter, last)) != std::string::npos)
	{
		token = str.substr(last, next - last);
		if (!token.empty())
			tokens.push_back(token);
		last = next + 1;
	}
	if (last != str.length())
	{
		token = str.substr(last);
		tokens.push_back(token);
	}
	return (tokens);
}

std::string Utils::size_t_to_str(size_t n)
{
	std::stringstream ss;

	ss << n;
	return (ss.str());
}

std::string Utils::str_trim(const std::string &str)
{
	size_t str_start, str_end;

	str_start = str.find_first_not_of(Utils::whitespace);
	str_end = str.find_last_not_of(Utils::whitespace);
	return (str.substr(str_start, str_end - str_start + 1));
}


