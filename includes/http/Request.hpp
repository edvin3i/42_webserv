#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../logger/Logger.hpp"
#include "Message.hpp"
#include "RequestLine.hpp"
#include "StatusCode.hpp"
#include "Utils.hpp"
#include "BodyPart.hpp"
#include <string>
#include <vector>
#include <cstdio>
#include <iostream>
#include <sstream>

class Request : protected Message<RequestLine>
{
public:

	Request(Logger& logger);
	~Request();
	Request(const Request & other);
	Request &operator=(const Request & other);

	void print() const;
	bool error() const;
	enum e_status_code getErrorCode() const;
	const RequestLine& getStartLine() const;
	const Headers& getHeaders() const;
	const Body& getBody() const;
	void setRequestLine(const std::string& str);
	void setHeaders(const std::vector<std::string>& fields);
	void setBody(const std::string& str, size_t content_length, bool is_chunked);
	void setError(enum e_status_code code);
	void _check_headers();

private:
	Logger &_logger;
	Request();
	enum e_status_code _error_code;
	bool _error;

};

#endif
