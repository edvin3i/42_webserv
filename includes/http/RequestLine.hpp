#ifndef REQUEST_LINE_HPP
#define REQUEST_LINE_HPP

#include <string>
#include <iostream>
#include <algorithm>

class RequestLine
{
public:
	RequestLine();
	RequestLine(const std::string&);
	~RequestLine();
	RequestLine(const RequestLine&);
	RequestLine& operator=(const RequestLine&);

	//For test
	void print() const;
private:
	void _parse_request_line(const std::string&);
	void _check_request_line() const;
private:
	std::string _method;
	std::string _request_target;
	std::string _http_version;
	static const size_t _max_request_line_length;
	static const size_t _nb_allowed_methods;
	static const std::string _allowed_methods[];
};

#endif
