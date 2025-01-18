#include "../../includes/http/FieldValue.hpp"

FieldValue::FieldValue()
: _value(), _parameters()
{
}

FieldValue::FieldValue(const std::string& str)
: _value(), _parameters()
{
	_parse_value(str);
}

void FieldValue::_handle_quote(const std::string& str, std::string& text, size_t& i)
{
	i += 1;
	while (i < str.length())
	{
		switch (str[i])
		{
			case '"':
				i += 1;
				return ;
			case '\\':
			{
				size_t next = i + 1;

				if (next < str.length())
					text.push_back(str[next]);
				i += 2;
				break ;
			}
			default:
				text.push_back(str[i]);
				i += 1;
				break ;
		}
	}
	throw (STATUS_BAD_REQUEST);
}

void FieldValue::_parse_value(const std::string & str)
{
	size_t i = 0;

	while (i < str.length())
	{
		switch (str[i])
		{
			case '\r': case '\n': case '\0':
				throw (STATUS_BAD_REQUEST);
			case ' ': case '\t':
			{
				std::string current_whitespace;

				while (Utils::is_whitespace(str[i]))
				{
					current_whitespace.push_back(str[i]);
					i += 1;
				}
				if (str[i] != ';')
					_value.append(current_whitespace);
				break ;
			}
			case '"':
			{
				if (i != 0)
					throw (STATUS_BAD_REQUEST);
				_handle_quote(str, _value, i);

				if (Utils::is_whitespace(str[i]) || str[i] == ';')
				{
					while (Utils::is_whitespace(str[i]))
						i += 1;
					if (str[i] != ';')
						throw (STATUS_BAD_REQUEST);
				}
				else
					throw (STATUS_BAD_REQUEST);
				break ;
			}
			case '(':
			{
				int depth = 1;
				i += 1;
				while (true)
				{
					if (i == str.length())
						throw (STATUS_BAD_REQUEST);
// <<<<<<< antonin
					if (str[i] == ')')
						depth -= 1;
					else if (str[i] == '(')
						depth += 1;
					if (str[i] == ')' && depth == 0)
					{
						i += 1;
						break ;
					}
// =======
					_parse_state = STATE_PARAM_VALUE;
					i += 1;
					break ;
				default:
					if (_parse_state == STATE_PARAM_NAME)
						name.push_back(str[i]);
					else if (_parse_state == STATE_PARAM_VALUE)
						value.push_back(str[i]);
// >>>>>>> master
					i += 1;
				}
				break ;
			}
			case ';':
			{
				size_t next = i + 1;
				if (next == str.length())
					throw (STATUS_BAD_REQUEST);
				_parse_parameters(Utils::str_trim(str.substr(next)));
				if (_value.empty())
					throw (STATUS_BAD_REQUEST);
				return ;
			}
			default:
				_value.push_back(str[i]);
				i += 1;
				break ;
		}
	}
}

void FieldValue::_parse_parameters(const std::string& str)
{
	std::string name, value;
	enum parse_state _parse_state = STATE_PARAM_NAME;

	size_t i = 0;
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
			case '"':
				if (_parse_state == STATE_PARAM_NAME)
				{
					if (!name.empty())
						throw (STATUS_BAD_REQUEST);
					_handle_quote(str, name, i);
					if (str[i] != '=')
						throw (STATUS_BAD_REQUEST);
				}
				else if (_parse_state == STATE_PARAM_VALUE)
				{
					if (!value.empty())
						throw (STATUS_BAD_REQUEST);
					_handle_quote(str, value, i);
					if (Utils::is_whitespace(str[i]) || str[i] == ';')
					{
						while (Utils::is_whitespace(str[i]))
							i += 1;
						if (str[i] != ';')
							throw (STATUS_BAD_REQUEST);
					}
				}
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

// FieldValue::FieldValue(const std::string& str)
// : _value(), _parameters()
// {
// 	std::string name, value;
// 	enum parse_state _parse_state = STATE_PARAM_NAME;
// 	size_t i = 0;
// 	while (i < str.length())
// 	{
// 		switch (str[i])
// 		{
// 			case ' ': case '\t':
// 				while (i < str.length() && Utils::is_whitespace(str[i]))
// 					i += 1;
// 				if (name.empty() && value.empty())
// 					continue ;
// 				else if ((str[i] == ';' || i == str.length()) && !name.empty() && !value.empty())
// 					continue ;
// 				else
// 					throw (STATUS_BAD_REQUEST);
// 				break ;
// 			case ';':
// 				if (_parse_state == STATE_PARAM_NAME || name.empty() || value.empty())
// 					throw (STATUS_BAD_REQUEST);
// 				_parameters[name] = value;
// 				name.clear();
// 				value.clear();
// 				_parse_state = STATE_PARAM_NAME;
// 				i += 1;
// 				break ;
// 			case '=':
// 				if (_parse_state == STATE_PARAM_VALUE || name.empty())
// 					throw (STATUS_BAD_REQUEST);
// 				_parse_state = STATE_PARAM_VALUE;
// 				i += 1;
// 				break ;
// 			default:
// 				if (_parse_state == STATE_PARAM_NAME)
// 					name.push_back(str[i]);
// 				else if (_parse_state = STATE_PARAM_VALUE)
// 					value.push_back(str[i]);
// 				i += 1;
// 		}
// 	}
// 	_parameters[name] = value;
// }

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

void FieldValue::addParameters(const std::string &param_name, const std::string &param_value)
{
	_parameters.insert(Parameter(param_name, param_value));
}
