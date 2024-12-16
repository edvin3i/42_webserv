#include "../../includes/http/Request.hpp"


Request::Request(const std::string& str)
: Message<RequestLine>()
{
	_parse(str);
}

Request::~Request() {}

Request::Request(const Request& other)
: Message<RequestLine>(other)
{}

Request& Request::operator=(const Request& other)
{
	if (this != &other)
	{
		Message<RequestLine>::operator=(other);
	}
	return (*this);
}

std::vector<std::string> split(std::string str, std::string delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos)
	{
		token = str.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}
	res.push_back(str.substr(pos_start));
	return (res);
}

void Request::_parse(const std::string &str)
{
	std::vector<std::string> request_lines = split(str, "\r\n");

	if (request_lines.size() < 2)
		throw (0);
	start_line = RequestLine(request_lines[0]);

	size_t i = 1;
	while (i < request_lines.size() && !request_lines[i].empty())
	{
		_parse_header(request_lines[i]);
		i += 1;
	}
	_parse_body(request_lines[i + 1]);
}

void Request::_parse_header(const std::string &str)
{
	std::string field_name, field_value;
	size_t colon_pos, field_value_pos_start, field_value_pos_end;

	colon_pos = str.find(':');
	field_name = str.substr(0, colon_pos);
	if (field_name.empty() || field_name.find(' ') != std::string::npos)
		throw (0);
	field_value_pos_start = str.find_first_not_of(' ', colon_pos + 1);
	field_value_pos_end = str.find_last_not_of(' ', colon_pos + 1);
	field_value = str.substr(field_value_pos_start, field_value_pos_end - field_value_pos_start);
	if (field_value.empty() || field_value.find(' ') != std::string::npos)
		throw (0);
	headers.insert(std::pair<std::string, std::string>(field_name, field_value));
}

std::vector<std::string> Request::_split_headers_line(const std::string& str)
{
	std::vector<std::string> headers_line;
	size_t old_pos = 0;
	size_t pos = 0;
	std::string line;
	while ((pos = str.find("\r\n", pos)) != std::string::npos)
	{
		line = str.substr(old_pos, pos - old_pos);
		headers_line.push_back(line);
		pos = pos + 2;
		old_pos = pos;
	}
	return (headers_line);
}

void Request::_parse_body(const std::string& str)
{
	bool is_transfer_encoding = headers.find("Transfer-Encoding") != headers.end();
	bool is_content_length = headers.find("Content-Length") != headers.end();

	if (is_transfer_encoding && is_content_length)
	{
		throw (0); //close connection after responding to avoid the potential attacks
	}
	else if (is_transfer_encoding)
	{
		throw (0); // Not implemented
	}
	else if(is_content_length)
	{
		sscanf(headers["Content-Length"].c_str(), "%zu", &body_size); // Maybe scanf breaks the rules
		body = str.substr(0, body_size);
	}
}

void Request::print() const
{
	std::clog << "REQUEST LINE: ";
	start_line.print();
	std::clog << "\n\n";

	std::clog << "HEADERS: \n";
	for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << "header-name: " << it->first << ", header-value: " << it->second << '\n';
	std::clog << "\n\n";

	std::clog << "BODY: \n";
	if (body.empty())
		std::clog << "empty body\n";
	else
		std::clog << body << '\n';
}
