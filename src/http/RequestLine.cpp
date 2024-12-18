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
: _method(), _request_target(), _http_version()
{
	_init();
}

RequestLine::RequestLine(const std::string& line)
{
	_init();
	if (line.length() > _max_request_line_length)
		throw (400);
	_parse_request_line(line);
	_check_request_line();
}

void RequestLine::_parse_request_line(const std::string& line)
{
	size_t i = 0;
	size_t space_pos;

	space_pos = line.find(' ');
	if (space_pos == std::string::npos)
		throw(400);
	_method = line.substr(i, space_pos - i);
	i = space_pos + 1;
	space_pos = line.find(' ', i);
	if (space_pos == std::string::npos)
		throw(400);
	_request_target = line.substr(i, space_pos - i);
	i = space_pos + 1;
	space_pos = line.find(' ', i);
	if (space_pos != std::string::npos)
		throw(400);
	_http_version = line.substr(i, std::string::npos);
}

void RequestLine::_check_request_line() const
{
	if (_method.length() > _max_method_length)
		throw (501);
	bool is_method_exist = false;
	for (size_t i = 0; i < _nb_allowed_methods; ++i)
		if (strcmp(_method.c_str(), _allowed_methods[i]) == 0)
			is_method_exist = true;
	if (is_method_exist == false)
		throw (501); // Not implemented
	if (_http_version != "HTTP/1.1")
		throw (505); // Version Not Supported
}

RequestLine::~RequestLine() {}

RequestLine::RequestLine(const RequestLine& other)
: _method(other._method), _request_target(other._request_target), _http_version(other._http_version)
{}

RequestLine& RequestLine::operator=(const RequestLine& other)
{
	if (this != &other)
	{
		_method = other._method;
		_request_target = other._request_target;
		_http_version = other._http_version;
	}
	return (*this);
}

void RequestLine::print() const
{
	std::clog << "method: " << _method << ", request-target: " << _request_target << ", HTTP-version: " << _http_version;
}
