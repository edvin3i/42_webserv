#include "../../includes/http/Field.hpp"

const size_t Field::_max_field_length = 1024;

Field::Field()
: std::pair<std::string, std::vector<FieldValue> >()
{}

Field::Field(const std::string& str)
: std::pair<std::string, std::vector<FieldValue> >()
{
	_parse_field(str);
}

Field::Field(const std::string& name, const std::vector<FieldValue>& values)
: std::pair<std::string, std::vector<FieldValue> >(name, values)
{}

Field::~Field() {}

Field::Field(const Field& other)
: std::pair<std::string, std::vector<FieldValue> >(other)
{}

Field& Field::operator=(const Field& other)
{
	if (this != &other)
	{
		std::pair<std::string, std::vector<FieldValue> >::operator=(other);
	}
	return (*this);
}

void Field::_parse_field(const std::string &str)
{
	std::string field_value, field_value_trim;
	size_t colon_pos;

	if (str.length() > _max_field_length)
		throw (STATUS_BAD_REQUEST);
	colon_pos = str.find(':');
	first = str.substr(0, colon_pos);
	if (first.empty() || Utils::all_of(first.begin(), first.end(), Utils::is_whitespace))
		throw (STATUS_BAD_REQUEST);
	field_value = str.substr(colon_pos + 1);
	field_value_trim = Utils::str_trim(field_value);
	_parse_field_list(field_value_trim);
}

void Field::_parse_field_list(const std::string& str)
{
	size_t nb_non_empty_element = 0;
	std::string element;
	size_t i = 0;

	while (i < str.length())
	{
		switch (str[i])
		{
			case ' ': case '\t':
				if (element.empty())
				{
					i += 1;
				}
				else
				{
					std::string current_whitespace;
					while (Utils::is_whitespace(str[i]))
					{
						current_whitespace.push_back(str[i]);
						i += 1;
					}
					if (str[i] != ',')
						element.append(current_whitespace);
				}
				break ;
			case ',':
				if (!element.empty() && !Utils::all_of(element.begin(), element.end(), Utils::is_whitespace))
				{
					second.push_back(FieldValue(Utils::str_trim(element)));
					nb_non_empty_element += 1;
				}
				element.clear();
				i += 1;
				break ;
			default:
				element.push_back(str[i]);
				i += 1;
		}
	}
	if (!element.empty() && !Utils::all_of(element.begin(), element.end(), Utils::is_whitespace))
	{
		second.push_back(FieldValue(Utils::str_trim(element)));
		nb_non_empty_element += 1;
	}
	if (nb_non_empty_element == 0)
		throw (STATUS_BAD_REQUEST);
}



size_t Field::count_values() const
{
	return (second.size());
}
