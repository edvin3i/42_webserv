#ifndef REQUEST_LINE_HPP
#define REQUEST_LINE_HPP

#include "StatusCode.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <map>
#include "Utils.hpp"
#include "Uri.hpp"
#include "Method.hpp"

typedef enum e_target_form
{
	FORM_ORIGIN,
	FORM_ABSOLUTE,
	NB_FORM
} TargetForm;


class RequestLine
{
public:
	RequestLine();
	RequestLine(const std::string & str);
	~RequestLine();
	RequestLine(const RequestLine&);
	RequestLine& operator=(const RequestLine&);

	void print() const;
	Method getMethod() const;
	const Uri& getUri() const;

private:
	void _parse_request_line(const std::string&);
	void _parse_method(const std::string&);
	void _parse_version(const std::string&);
	void _parse_uri(const std::string&);

	Method _method;
	Uri _uri;
	std::string _http_version;
	static const size_t _max_request_line_length;
};

#endif
