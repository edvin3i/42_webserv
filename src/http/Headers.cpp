#include "../../includes/http/Headers.hpp"

const size_t Headers::_max_header_length = 1024;

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
	for (size_t i = 0; i < fields.size(); ++i)
	{
		field_trim = Utils::str_trim(fields[i]);
		_parse_field(field_trim);
	}
}

void Headers::_parse_field(const std::string &str)
{
	std::string field_name, field_value, field_value_trim;
	size_t colon_pos;

	if (str.length() > _max_header_length)
		throw (STATUS_BAD_REQUEST);
	colon_pos = str.find(':');
	field_name = str.substr(0, colon_pos);
	if (field_name.empty() || Utils::is_whitespace(field_name[field_name.length() - 1]))
		throw (400);
	field_value = str.substr(colon_pos + 1);
	field_value_trim = Utils::str_trim(field_value);
	_parse_field_value(field_value_trim, field_name);
}


void Headers::_parse_field_value(const std::string & str, const std::string & field_name)
{
	std::string element;
	size_t i = 0;
	size_t nb_non_empty_element = 0;

	while (i < str.length())
	{
		switch (str[i])
		{
			case '\r': case '\n': case '\0':
				throw (400);
			case ' ': case '\t':
				if (element.empty())
				{
					while (Utils::is_whitespace(str[i]))
						i += 1;
				}
				else
				{
					size_t nb_whitespace = 0;
					while (Utils::is_whitespace(str[i]))
					{
						nb_whitespace += 1;
						i += 1;
					}
					if (!_is_delimiter(str[i]))
						element.append(nb_whitespace, ' ');
				}
				break ;
			case '\"':
				_handle_quoted_str(str, i, element);
				break ;
			case ',':
				if (!element.empty())
					nb_non_empty_element += 1;
				insert(std::pair<std::string, std::string>(field_name, element));
				element.clear();
				i += 1;
				break ;
			case '(':
			{
				int depth = 0;
				while (true)
				{
					if (i == str.length())
						throw (STATUS_BAD_REQUEST);
					if (str[i] == '(')
						depth++;
					else if (str[i] == ')')
						depth--;
					if (str[i] == ')' && (depth == 0))
					{
						i += 1;
						break ;
					}
					if (depth < 0)
						throw (STATUS_BAD_REQUEST);
					i += 1;
				}
				break ;
			}
			default:
				element.push_back(str[i]);
				i += 1;
				break ;
		}
	}
	if (!element.empty())
		nb_non_empty_element += 1;
	if (nb_non_empty_element == 0)
		throw (STATUS_BAD_REQUEST);
	insert(Field(field_name, FieldValue(element)));
}

bool Headers::_is_delimiter(char c)
{
	const std::string delimiters = "(),/:;<=>?@[\\]{}";

	return (delimiters.find(c) != std::string::npos);
}

void Headers::_handle_quoted_str(const std::string& str, size_t& i, std::string& element)
{
	if (!element.empty())
		throw (400);
	i += 1;
	while (1)
	{
		if (i == str.length())
			throw (400);
		if (str[i] == '\"')
		{
			if (str.length() == (i + 1) || _is_delimiter(str[i + 1]) || Utils::is_whitespace(str[i + 1]))
			{
				i += 1;
				return ;
			}
			else
				throw (400);
		}
		element.push_back(str[i]);
		i += 1;
	}
}
