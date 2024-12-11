#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Message.hpp"
#include "RequestLine.hpp"
#include <string>
#include <vector>
#include <cstdio>

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
	void _parse_headers(const std::string&);
	void _parse_body(const std::string&);
	std::vector<std::string> _split_headers_line(const std::string&);
};

#endif
