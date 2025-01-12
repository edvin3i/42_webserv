#include "../../includes/http/RequestLine.hpp"

const size_t RequestLine::_max_request_line_length = 8192;

const char *RequestLine::_allowed_methods[] = {"GET", "POST", "DELETE"};

const size_t RequestLine::_nb_allowed_methods = sizeof(_allowed_methods) / sizeof(_allowed_methods[0]);

size_t RequestLine::_max_method_length = 0;

void RequestLine::_init()
{
	for (size_t i = 0; i < _nb_allowed_methods; ++i)
	{
		size_t len = strlen(_allowed_methods[i]);
		if (len > _max_method_length)
			_max_method_length = len;
	}
}

RequestLine::RequestLine()
: _method(), _uri(), _http_version()
{
	_init();
}

RequestLine::RequestLine(const std::string& line)
{
	_init();
	if (line.length() > _max_request_line_length)
		throw (STATUS_BAD_REQUEST);
	_parse_request_line(line);
}

void RequestLine::_parse_method(const std::string& str)
{
	if (str.length() > _max_method_length)
		throw (STATUS_NOT_IMPLEMENTED);
	bool is_method_exist = false;
	for (size_t i = 0; i < _nb_allowed_methods; ++i)
		if (strcmp(str.c_str(), _allowed_methods[i]) == 0)
			is_method_exist = true;
	if (is_method_exist == false)
		throw (STATUS_NOT_IMPLEMENTED); // Not implemented
	_method = str;
}

void RequestLine::_parse_version(const std::string& str)
{
	if (str != "HTTP/1.1")
		throw (STATUS_VERSION_NOT_SUPPORTED); // Version Not Supported
	_http_version = str;
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
	_uri = line.substr(i, space_pos - i);
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
	std::clog << "method: " << _method << ", request-target: " << _uri << ", HTTP-version: " << _http_version;
}
