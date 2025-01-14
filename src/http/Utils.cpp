#include "../../includes/http/Utils.hpp"

bool Utils::is_whitespace(char c)
{
	return (whitespace.find(c) != std::string::npos);
}
