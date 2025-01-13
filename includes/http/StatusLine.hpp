#ifndef STATUS_LINE_HPP
#define STATUS_LINE_HPP

#include <string>
#include <sstream>
#include <map>

#include "StatusCode.hpp"

class StatusLine
{
public:
	StatusLine();
	StatusLine(enum e_status_code status_code);
	~StatusLine();
	StatusLine(const StatusLine & other);
	StatusLine& operator=(const StatusLine & other);
	std::string toHtml() const;
	static std::map<int, std::string> _status_code_message;
private:
	static void _init_status_code_message();
	static bool _is_status_code_message_init;
	static const std::string _http_version;
	enum e_status_code _status_code;
};

#endif
