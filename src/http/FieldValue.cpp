#include "../../includes/http/FieldValue.hpp"

FieldValue::FieldValue()
: _value(), _parameters()
{
}

FieldValue::FieldValue(const std::string& str)
: _value(), _parameters()
{
	size_t semi_colon_pos = str.find(';');

	if (semi_colon_pos == std::string::npos)
		_value = str;
	else
	{
		std::string name, value;
		enum parse_state _parse_state = STATE_PARAM_NAME;
		size_t value_length = semi_colon_pos;

		value_length -= 1;
		while (value_length > 0 && Utils::is_whitespace(str[value_length]))
			value_length -= 1;
		if (value_length == 0)
			throw (STATUS_BAD_REQUEST);
		_value = str.substr(0, value_length + 1);
		// for (size_t i = semi_colon_pos + 1; i < str.length(); ++i)
		size_t i = semi_colon_pos + 1;
		while (i < str.length())
		{
			switch (str[i])
			{
				case ' ': case '\t':
					while (i < str.length() && Utils::is_whitespace(str[i]))
						i += 1;
					if (name.empty() && value.empty())
						continue ;
					else if ((str[i] == ';' || i == str.length()) && !name.empty() && !value.empty())
						continue ;
					else
						throw (STATUS_BAD_REQUEST);
					break ;
				case ';':
					if (_parse_state == STATE_PARAM_NAME || name.empty() || value.empty())
						throw (STATUS_BAD_REQUEST);
					_parameters[name] = value;
					name.clear();
					value.clear();
					_parse_state = STATE_PARAM_NAME;
					i += 1;
					break ;
				case '=':
					if (_parse_state == STATE_PARAM_VALUE || name.empty())
						throw (STATUS_BAD_REQUEST);
					_parse_state = STATE_PARAM_VALUE;
					i += 1;
					break ;
				default:
					if (_parse_state == STATE_PARAM_NAME)
						name.push_back(str[i]);
					else if (_parse_state = STATE_PARAM_VALUE)
						value.push_back(str[i]);
					i += 1;
			}
		}
		_parameters[name] = value;
	}
}

FieldValue::~FieldValue()
{}

FieldValue::FieldValue(const FieldValue& other)
: _value(other._value), _parameters(other._parameters)
{}

FieldValue& FieldValue::operator=(const FieldValue& other)
{
	if (this != &other)
	{
		_value = other._value;
		_parameters = other._parameters;
	}
	return (*this);
}

const std::string& FieldValue::getValue() const
{
	return (_value);
}

const std::map<std::string, std::string>& FieldValue::getParameters() const
{
	return (_parameters);
}

void FieldValue::setValue(const std::string &value)
{
	_value = value;
}
