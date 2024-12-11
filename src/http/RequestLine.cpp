#include "../../includes/http/RequestLine.hpp"

const size_t RequestLine::_max_request_line_length = 8192;

const size_t RequestLine::_nb_allowed_methods = 3;

const std::string RequestLine::_allowed_methods[_nb_allowed_methods] = {"GET", "POST", "DELETE"};

RequestLine::RequestLine()
: _method(), _request_target(), _http_version()
{}

RequestLine::RequestLine(const std::string& line)
{

	if (line.length() > _max_request_line_length)
		throw (0);
	_parse_request_line(line);
	_check_request_line();
}

void RequestLine::_parse_request_line(const std::string& line)
{
	size_t i = 0;
	size_t space_pos;

	space_pos = line.find(' ');
	if (space_pos == std::string::npos)
		throw(0);
	_method = line.substr(i, space_pos - i);
	i = space_pos + 1;
	space_pos = line.find(' ', i);
	if (space_pos == std::string::npos)
		throw(0);
	_request_target = line.substr(i, space_pos - i);
	i = space_pos + 1;
	space_pos = line.find(' ', i);
	if (space_pos != std::string::npos)
		throw(0);
	_http_version = line.substr(i, std::string::npos);
}

void RequestLine::_check_request_line() const
{
	const std::string *first_method = _allowed_methods;
	const std::string *last_method = _allowed_methods + _nb_allowed_methods;

	if (std::find(first_method, last_method, _method) == last_method)
		throw (0);
	//check request target
	if (_http_version != "HTTP/1.1")
		throw (0);
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
