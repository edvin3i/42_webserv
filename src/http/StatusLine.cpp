#include "../../includes/http/StatusLine.hpp"

static const std::string _http_version = "HTTP/1.1";

StatusLine::StatusLine()
: _status_code(200), _reason_phrase("OK")
{
}

StatusLine::~StatusLine() {}

StatusLine::StatusLine(int status_code, const std::string& reason_phrase)
: _status_code(status_code), _reason_phrase(reason_phrase)
{
}

StatusLine::StatusLine(const StatusLine& other)
: _status_code(other._status_code), _reason_phrase(other._reason_phrase)
{
}

StatusLine& StatusLine::operator=(const StatusLine& other)
{
	if (this != &other)
	{
		_status_code = other._status_code;
		_reason_phrase = other._reason_phrase;
	}
	return (*this);
}
