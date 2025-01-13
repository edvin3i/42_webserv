#ifndef REQUEST_LINE_HPP
#define REQUEST_LINE_HPP

#include "StatusCode.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>

class RequestLine
{
public:
	RequestLine();
	RequestLine(const std::string & str);
	~RequestLine();
	RequestLine(const RequestLine&);
	RequestLine& operator=(const RequestLine&);

	void print() const;
	const std::string& getMethod() const;
	const std::string& getUri() const;
private:
	void _init();
	void _parse_request_line(const std::string&);
	void _parse_method(const std::string&);
	void _parse_version(const std::string&);

	std::string _method;
	std::string _uri;
	std::string _http_version;
	static const size_t _max_request_line_length;
	static size_t _max_method_length;
	static const char *_allowed_methods[];
	static const size_t _nb_allowed_methods;
};

#endif
