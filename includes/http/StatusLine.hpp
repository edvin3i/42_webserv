#ifndef STATUS_LINE_HPP
#define STATUS_LINE_HPP

#include <string>

class StatusLine
{
public:
	StatusLine(std::string http_ver, int status_code, std::string reason_phrase);
	~StatusLine();
	StatusLine(const StatusLine & other);
	StatusLine& operator=(const StatusLine & other);
private:
	std::string _http_version;
	int _status_code;
	std::string _reason_phrase;
};

#endif
