#include "../../includes/http/Headers.hpp"

Headers::Headers()
: std::multimap<std::string, FieldValue>()
{
}

Headers::Headers(const std::vector<std::string>& fields)
: std::multimap<std::string, FieldValue>()
{
	_parse_fields(fields);
}

Headers::~Headers()
{}

Headers::Headers(const Headers& other)
: std::multimap<std::string, FieldValue>(other)
{}

Headers& Headers::operator=(const Headers& other)
{
	if (this != &other)
	{
		std::multimap<std::string, FieldValue>::operator=(other);
	}
	return (*this);
}

void Headers::_parse_fields(const std::vector<std::string>& fields)
{
	std::string field_trim;
	Field current_field;
	for (size_t i = 0; i < fields.size(); ++i)
	{
		field_trim = Utils::str_trim(fields[i]);
		current_field = Field(field_trim);
		if (count(current_field.first))
			throw (STATUS_BAD_REQUEST);
		for (size_t j = 0; j < current_field.count_values(); ++j)
			insert(std::pair<std::string, FieldValue>(current_field.first, current_field.second[j]));
	}
}

bool Headers::_is_delimiter(char c)
{
	const std::string delimiters = "(),/:;<=>?@[\\]{}";

	return (delimiters.find(c) != std::string::npos);
}

void Headers::_handle_quoted_str(const std::string& str, size_t& i, std::string& element)
{
	if (!element.empty())
		throw (STATUS_BAD_REQUEST);
	i += 1;
	while (1)
	{
		if (i == str.length())
			throw (STATUS_BAD_REQUEST);
		if (str[i] == '\"')
		{
			if (str.length() == (i + 1) || _is_delimiter(str[i + 1]) || Utils::is_whitespace(str[i + 1]))
			{
				i += 1;
				return ;
			}
			else
				throw (STATUS_BAD_REQUEST);
		}
		element.push_back(str[i]);
		i += 1;
	}
}
