#ifndef STATUS_LINE_HPP
#define STATUS_LINE_HPP

#include <string>

class StatusLine
{
public:
	StatusLine(std::string, int, std::string);
	~StatusLine();
	StatusLine(const StatusLine&);
	StatusLine& operator=(const StatusLine&);
private:
	std::string _http_version;
	int _status_code;
	std::string _reason_phrase;
};

#endif
