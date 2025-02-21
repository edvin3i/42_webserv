#include "../../includes/http/RequestLine.hpp"

const size_t RequestLine::_max_request_line_length = 8192;


RequestLine::RequestLine()
: _method(), _uri(), _http_version()
{
}

RequestLine::RequestLine(const std::string& line)
{
	if (line.length() > _max_request_line_length)
		throw (STATUS_BAD_REQUEST);
	_parse_request_line(line);
}

void RequestLine::_parse_method(const std::string& str)
{
	try
	{
		_method = Method(str);
	}
	catch (const std::exception& e)
	{
		throw (STATUS_NOT_IMPLEMENTED);
	}
}

void RequestLine::_parse_version(const std::string& str)
{
	if (str != "HTTP/1.1")
		throw (STATUS_VERSION_NOT_SUPPORTED); // Version Not Supported
	_http_version = str;
}

void RequestLine::_parse_uri(const std::string& str)
{
	for (size_t i = 0; i < str.length(); ++i)
		if (Utils::is_whitespace(str[i]))
			throw (STATUS_BAD_REQUEST);
	_uri = Uri(str);
}

void RequestLine::_parse_request_line(const std::string& line)
{
	size_t i = 0;
	size_t space_pos;

	space_pos = line.find(' ');
	if (space_pos == std::string::npos)
		throw (STATUS_BAD_REQUEST);
	_parse_method(line.substr(i, space_pos - i));
	i = space_pos + 1;
	space_pos = line.find(' ', i);
	if (space_pos == std::string::npos)
		throw (STATUS_BAD_REQUEST);
	_parse_uri(line.substr(i, space_pos - i));
	i = space_pos + 1;
	space_pos = line.find(' ', i);
	if (space_pos != std::string::npos)
		throw (STATUS_BAD_REQUEST);
	_parse_version(line.substr(i, std::string::npos));
}

RequestLine::~RequestLine() {}

RequestLine::RequestLine(const RequestLine& other)
: _method(other._method), _uri(other._uri), _http_version(other._http_version)
{}

RequestLine& RequestLine::operator=(const RequestLine& other)
{
	if (this != &other)
	{
		_method = other._method;
		_uri = other._uri;
		_http_version = other._http_version;
	}
	return (*this);
}

void RequestLine::print() const
{
	std::clog << "method: " << _method.toString() << ", request-target: " << _uri.getPath() << ", HTTP-version: " << _http_version;
}

Method RequestLine::getMethod() const
{
	return (_method);
}

const Uri& RequestLine::getUri() const
{
	return (_uri);
}

