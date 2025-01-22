#include "../../includes/http/BodyPart.hpp"

BodyPart::BodyPart(const std::string& body)
: _headers(), _body()
{
	_parse_body(body);
}

BodyPart::~BodyPart() {}

BodyPart::BodyPart(const BodyPart& other)
: _headers(other._headers), _body(other._body)
{}

BodyPart& BodyPart::operator=(const BodyPart& other)
{
	if (this != &other)
	{
		_body = other._body;
		_headers = other._headers;
	}
	return (*this);
}

void BodyPart::_split_body_part(const std::string& str, std::vector<std::string>& fields)
{
	const std::string delimiter = "\r\n";
	size_t pos_start = 0, pos_end, delim_len = 2;
	std::string field_line;

	while (1)
	{
		pos_end = str.find(delimiter, pos_start);
		if (pos_end == std::string::npos)
			throw (STATUS_BAD_REQUEST);
		field_line = str.substr(pos_start, pos_end - pos_start);
		if (field_line.empty())
		{
			_body = str.substr(pos_end + 2);
			break ;
		}
		fields.push_back(field_line);
		pos_start = pos_end + delim_len;
	}
}

void BodyPart::_parse_body(const std::string &str)
{
	std::vector<std::string> fields;

	_split_body_part(str, fields);
	_headers = Headers(fields);
	_check_headers();
}

void BodyPart::_check_headers()
{
	if (_headers.count(Headers::getTypeStr(HEADER_CONTENT_DISPOSITION)) != 1)
		throw (STATUS_BAD_REQUEST);
	const Parameters& parameters = _headers.find(Headers::getTypeStr(HEADER_CONTENT_DISPOSITION))->second.getParameters();
	if (parameters.count("name") != 1)
		throw (STATUS_BAD_REQUEST);
	const size_t nb_content_type = _headers.count(Headers::getTypeStr(HEADER_CONTENT_TYPE));

	if (nb_content_type > 1)
		throw (STATUS_BAD_REQUEST);
	if (nb_content_type == 0)
	{
		FieldValue field_value("text/plain");
		field_value.addParameters("charset", "US-ASCII");
		_headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_TYPE), field_value));
	}
}

const Headers& BodyPart::getHeaders() const
{
	return (_headers);
}

const std::string& BodyPart::getBody() const
{
	return (_body);
}
