#include "../includes/Utils.hpp"

const std::string Utils::whitespace = " \t";

bool Utils::is_whitespace(char c)
{
	return (whitespace.find(c) != std::string::npos);
}

std::vector<std::string> Utils::split_path(const std::string& str)
{
	std::vector<std::string> tokens;
	size_t last = 0, next = 0;
	std::string token;

	while ((next = str.find('/', last)) != std::string::npos)
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

std::vector<std::string> Utils::split(const std::string& str, const std::string& delimiter)
{
	std::vector<std::string> tokens;

	if (delimiter.empty())
		throw std::invalid_argument("Delimiter cannot be empty");

	size_t last = 0, next = 0;

	while ((next = str.find(delimiter, last)) != std::string::npos)
	{
		tokens.push_back(str.substr(last, next - last));
		last = next + delimiter.length();
	}
	tokens.push_back(str.substr(last));

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


int Utils::stoi(const std::string& str)
{
	std::istringstream iss(str);
	int value = 0;

	iss >> value;

	if (iss.fail() || !iss.eof())
		throw (std::invalid_argument("Invalid argument"));
	return (value);
}

std::string Utils::to_lowercase(const std::string& str)
{
	std::string result(str);

	for (size_t i = 0; i < result.length(); ++i)
		result[i] = std::tolower(result[i]);
	return (result);
}
