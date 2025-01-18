#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../logger/Logger.hpp"
#include "Message.hpp"
#include "RequestLine.hpp"
#include "StatusCode.hpp"
#include "Utils.hpp"
#include <string>
#include <vector>
#include <cstdio>
#include <iostream>
#include <sstream>

class Request : public Message<RequestLine>
{
public:

	Request(Logger & logger, const std::string & str);
	~Request();
	Request(const Request & other);
	Request &operator=(const Request & other);

	void print() const;
	// std::pair<Headers::iterator, Headers::iterator> getFieldValue(const std::string&);
	bool error() const;
	enum e_status_code getErrorCode() const;
private:
	Logger &_logger;
	Request();
	enum e_status_code _error_code;
	bool _error;
	std::string _str_trim(const std::string &str) const;

	void _parse(const std::string&);
	void _parse_header(const std::string&);
	void _parse_field_value(const std::string &str, const std::string& field_name);
	void _handle_quoted_str(const std::string& str, size_t& i, std::string& element) const;
	void _parse_headers(std::vector<std::string>& header_lines);
	void _check_headers();
	void _parse_body(const std::string & str);
	void _split_request(std::string str, std::string & request_line, std::vector<std::string> & headers_line, std::string & body);
	void _decode_chunked(const std::string & str);


};

#endif
