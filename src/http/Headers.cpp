#include "../../includes/http/Headers.hpp"

std::vector<std::string> Headers::_type_to_str;

bool Headers::is_init = false;

void Headers::_init()
{
	if (is_init)
		return ;
	_type_to_str.resize(NB_HEADER_TYPE);
	_type_to_str[HEADER_CONTENT_TYPE] = "content-type";
	_type_to_str[HEADER_CONTENT_LENGTH] = "content-length";
	_type_to_str[HEADER_LOCATION] = "location";
	_type_to_str[HEADER_HOST] = "host";
	_type_to_str[HEADER_CONTENT_DISPOSITION] = "content-disposition";
}

std::string Headers::getTypeStr(HeaderType type)
{
	return (std::string(_type_to_str[type]));
}

Headers::Headers()
: std::multimap<std::string, FieldValue>()
{
	_init();
}

Headers::Headers(const std::vector<std::string>& fields)
: std::multimap<std::string, FieldValue>()
{
	_init();
	_parse_fields(fields);
}

Headers::~Headers()
{}

Headers::Headers(const Headers& other)
: std::multimap<std::string, FieldValue>(other)
{
	_init();
}

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


