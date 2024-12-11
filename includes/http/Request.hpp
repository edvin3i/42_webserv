#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>

enum Method
{
	METHOD_GET,
	METHOD_POST,
	METHOD_DELETE,
	NB_METHOD
};



class Request
{
public:
	Request(const std::string&);
	~Request();
	Request(const Request& other);
	Request& operator=(const Request& other);
private:
	Request();
	void parse_request(const std::string&);
private:
	std::string _request_line;
	Method _method;
	std::string _request_target;
	std::string _version;
	std::vector<std::string, std::string> _headers;
	std::string _body;
	size_t _body_size;
};



#endif
