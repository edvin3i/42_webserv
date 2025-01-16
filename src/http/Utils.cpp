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
		tokens.push_back(token);
		last = next + 1;
	}
	return (tokens);
}
