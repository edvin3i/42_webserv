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
	void _parse_header(const std::string&);
	void _parse_body(std::vector<std::string>&, size_t);
	std::vector<std::string> _split_headers_line(const std::string&);
};

#endif
