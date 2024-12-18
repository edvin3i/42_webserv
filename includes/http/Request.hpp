#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Message.hpp"
#include "RequestLine.hpp"
#include <string>
#include <vector>
#include <cstdio>
#include <iostream>
#include <sstream>

class Request : public Message<RequestLine>
{
public:
	Request(const std::string&);
	~Request();
	Request(const Request&);
	Request& operator=(const Request&);

	void print() const;
private:
	Request();
	void _parse(const std::string&);
	void _parse_header(const std::string&);
	void _parse_headers(std::vector<std::string>& header_lines);
	void _parse_body(const std::string&);
	void _split_request(std::string str, std::string& request_line, std::vector<std::string>& headers_line, std::string& body);
	void _decode_chunked(const std::string& str);
};

#endif
