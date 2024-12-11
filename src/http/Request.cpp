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

void Request::_parse(const std::string &str)
{
	std::string request_line_str, headers_str, body_str;
	size_t i = 0;
	size_t crlf_pos;

	crlf_pos = str.find("\r\n", i);
	if (crlf_pos == std::string::npos)
		throw (0);
	request_line_str = str.substr(i, crlf_pos);
	start_line = RequestLine(request_line_str);
	i = crlf_pos + 2;
	crlf_pos = str.find("\r\n\r\n", i);
	if (crlf_pos == std::string::npos)
		throw (0);
	headers_str = str.substr(i, crlf_pos - i + 2);
	_parse_headers(headers_str);
	i = crlf_pos + 4;
	_parse_body(str.substr(i));
}

void Request::_parse_headers(const std::string &str)
{
	std::vector<std::string> header_field_lines = _split_headers_line(str);

	for (size_t i = 0; i < header_field_lines.size(); ++i)
	{
		std::string line = header_field_lines[i];
		std::string field_name, field_value;
		size_t j = 0;
		while (j < line.length() && line[j] != ':')
		{
			field_name += line[j];
			if (line[j] == ' ')
				throw (0); //400 Bad Request
			j += 1;
		}
		if (j == line.length())
			throw (0);
		j += 1;
		while (j < line.length() && line[j] == ' ')
			j += 1;
		if (j == line.length())
			throw (0);
		while (j < line.length() && line[j] != ' ')
		{
			field_value += line[j];
			j += 1;
		}
		while (j < line.length() && line[j] == ' ')
			j += 1;
		if (j != line.length())
			throw (0);
		headers.insert(std::pair<std::string, std::string>(field_name, field_value));
	}
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
