#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Message.hpp"
#include "RequestLine.hpp"
#include "StatusCode.hpp"
#include <string>
#include <vector>
#include <cstdio>
#include <iostream>
#include <sstream>

class Request : public Message<RequestLine>
{
public:
	Request(const std::string & str);
	~Request();
	Request(const Request & other);
	Request &operator=(const Request & other);

	void print() const;
private:
	Request();
	void _parse(const std::string & str);
	void _parse_header(const std::string & str);
	void _parse_field_value(const std::string & str, const std::string & field_name);
	void _parse_headers(std::vector<std::string> & header_lines);
	void _check_headers() const;
	void _parse_body(const std::string & str);
	void _split_request(std::string str, std::string & request_line, std::vector<std::string> & headers_line, std::string & body);
	void _decode_chunked(const std::string & str);
};

#endif
