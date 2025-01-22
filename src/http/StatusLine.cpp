#include "../../includes/http/StatusLine.hpp"

const std::string StatusLine::_http_version = "HTTP/1.1";

bool StatusLine::_is_status_code_message_init = false;

std::map<int, std::string> StatusLine::_status_code_message;

void StatusLine::_init_status_code_message()
{
	if (_is_status_code_message_init)
		return ;
	_status_code_message[STATUS_OK] = "OK";
	_status_code_message[STATUS_CREATED] = "Created";
	_status_code_message[STATUS_NO_CONTENT] = "No Content";
	_status_code_message[STATUS_MOVED] = "Moved Permanently";
	_status_code_message[STATUS_BAD_REQUEST] = "Bad Request";
	_status_code_message[STATUS_FORBIDDEN] = "Forbidden";
	_status_code_message[STATUS_NOT_FOUND] = "Not Found";
	_status_code_message[STATUS_NOT_ALLOWED] = "Method Not Allowed";
	_status_code_message[STATUS_CONFLICT] = "Conflict";
	_status_code_message[STATUS_LENGTH_REQUIRED] = "Length Required";
	_status_code_message[STATUS_TOO_LARGE] = "Content Too Large";
	_status_code_message[STATUS_URI_TOO_LONG] = "URI Too Long";
	_status_code_message[STATUS_UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
	_status_code_message[STATUS_INTERNAL_ERR] = "Internal Server Error";
	_status_code_message[STATUS_NOT_IMPLEMENTED] = "Not Implemented";
	_status_code_message[STATUS_BAD_GATEWAY] = "Bad Gateway";
	_status_code_message[STATUS_GATEWAY_TIMEOUT] = "Gateway Timeout";
	_status_code_message[STATUS_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
	_is_status_code_message_init = true;
}

StatusLine::StatusLine()
: _status_code(STATUS_OK)
{
	_init_status_code_message();
}

StatusLine::~StatusLine() {}

StatusLine::StatusLine(enum e_status_code status_code)
: _status_code(status_code)
{
	_init_status_code_message();
}

StatusLine::StatusLine(const StatusLine& other)
: _status_code(other._status_code)
{
	_init_status_code_message();
}

StatusLine& StatusLine::operator=(const StatusLine& other)
{
	if (this != &other)
	{
		_status_code = other._status_code;
	}
	return (*this);
}

std::string StatusLine::toHtml() const
{
	std::stringstream ss;
	std::string message;

	try
	{
		message = _status_code_message.at(_status_code);
	}
	catch (const std::exception& e)
	{
		message = "No Message";
	}

	ss << _http_version;
	ss << ' ';
	ss << _status_code;
	ss << ' ';
	ss << message;
	return (ss.str());
}

std::string StatusLine::toString() const
{
	return (toHtml());
}
