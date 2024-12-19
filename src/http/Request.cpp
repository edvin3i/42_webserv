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

void Request::_split_request(std::string str, std::string& request_line, std::vector<std::string>& header_lines, std::string& body)
{
	const std::string delimiter = "\r\n";
	size_t pos_start = 0, pos_end, delim_len = 2;
	std::string header_line;

	pos_end = str.find(delimiter, pos_start);
	request_line = str.substr(pos_start, pos_end - pos_start);
	pos_start = pos_end + delim_len;
	if (str[pos_start] == ' ')
		throw (400); // reject message as invalid
	while (1)
	{
		pos_end = str.find(delimiter, pos_start);
		if (pos_end == std::string::npos)
			throw (400);
		header_line = str.substr(pos_start, pos_end - pos_start);
		if (header_line.empty())
		{
			body = str.substr(pos_end + 2);
			break ;
		}
		header_lines.push_back(header_line);
		pos_start = pos_end + delim_len;
	}
}

void Request::_parse(const std::string &str)
{
	std::string request_line, body;
	std::vector<std::string> header_lines;

	_split_request(str, request_line, header_lines, body);
	start_line = RequestLine(request_line);
	_parse_headers(header_lines);
	_check_headers();
	_parse_body(body);
}

void Request::_parse_headers(std::vector<std::string>& header_lines)
{
	for (size_t i = 0; i < header_lines.size(); ++i)
		_parse_header(header_lines[i]);
}

void Request::_parse_field_value(const std::string &str, std::vector<std::string>& field_value)
{
	std::string element;
	size_t i = 0;
	size_t nb_non_empty_element = 0;

	while (i < str.length())
	{
		switch (str[i])
		{
			case ' ':
				if (element.empty())
				{
					while (str[i] == ' ')
						i += 1;
				}
				else
				{
					size_t nb_space = 0;
					while (str[i] == ' ')
					{
						nb_space += 1;
						i += 1;
					}
					if (str[i] != ',')
						element.append(nb_space, ' ');
				}
				break ;
			case ',':
				field_value.push_back(element);
				element.clear();
				i += 1;
				break ;
			default:
				if (element.empty())
					nb_non_empty_element += 1;
				element.push_back(str[i]);
				i += 1;
				break ;
		}
	}
	field_value.push_back(element);
	if (nb_non_empty_element == 0)
		throw (400);
}

void Request::_parse_header(const std::string &str)
{
	std::string field_name, field_value_str_trim;
	std::vector<std::string> field_value;
	size_t colon_pos, field_value_pos_start, field_value_pos_end;

	colon_pos = str.find(':');
	field_name = str.substr(0, colon_pos);
	if (field_name.empty() || field_name.find(' ') != std::string::npos)
		throw (400);
	field_value_pos_start = str.find_first_not_of(' ', colon_pos + 1);
	field_value_pos_end = str.find_last_not_of(' ', str.length());
	field_value_str_trim = str.substr(field_value_pos_start, field_value_pos_end - field_value_pos_start + 1);
	_parse_field_value(field_value_str_trim, field_value);
	headers.insert(std::pair<std::string, std::vector<std::string> >(field_name, field_value));
}

void Request::_check_headers() const
{
	//respond with 400 when request message contains more than one Host header field line or a Host header field with an invalid field value
	if (headers.find("Host") == headers.end())
		throw (400);
}

void Request::_decode_chunked(const std::string& str)
{
	size_t length = 0, chunk_size;
	char *chunk_data;
	std::istringstream sstream(str);

	sstream >> std::hex >> chunk_size;
	sstream.ignore(1, '\r');
	sstream.ignore(1, '\n');
	while (chunk_size > 0)
	{
		chunk_data = new char[chunk_size + 1];
		sstream.read(chunk_data, chunk_size);
		chunk_data[chunk_size] = '\0';
		sstream.ignore(1, '\r');
		sstream.ignore(1, '\n');
		content.append(chunk_data);
		delete[] chunk_data;
		length += chunk_size;
		sstream >> std::hex >> chunk_size;
		sstream.ignore(1, '\r');
		sstream.ignore(1, '\n');
	}
	std::stringstream ss_content_length;
	ss_content_length << length;
	headers.insert(std::pair<std::string, std::vector<std::string> >("Content-Length", std::vector<std::string>(1, ss_content_length.str())));
	content_length = length;
	headers["Transfer-Encoding"] = std::vector<std::string>(1, "");
}

static void _read_size_t(const std::string& str, size_t& n)
{
	std::stringstream sstream(str);
	sstream >> n;
}

void Request::_parse_body(const std::string& str)
{
	Headers::iterator transfer_encoding_it = headers.find("Transfer-Encoding");
	Headers::iterator content_length_it = headers.find("Content-Length");

	if (str.empty())
		return ;
	if (transfer_encoding_it != headers.end() && content_length_it != headers.end())
	{
		throw (400);
		//close connection after responding
	}
	else if (transfer_encoding_it != headers.end())
	{
		if (transfer_encoding_it->second.size() == 1 && transfer_encoding_it->second[0] == "chunked")
			_decode_chunked(str);
		else
			throw (400); // Status not implemented
	}
	else if(content_length_it != headers.end())
	{
		size_t nb_value;

		switch (nb_value = headers["Content-Length"].size())
		{
			case '0':
				throw (411);
			case '1':
				_read_size_t(headers["Content-Length"][0], content_length);
				break ;
			default:
				size_t tmp;
				_read_size_t(headers["Content-Length"][0], content_length);
				for (size_t i = 1; i < nb_value; ++i)
				{
					_read_size_t(headers["Content-Length"][i], tmp);
					if (tmp != content_length)
						throw (411);
				}
				break ;
		}
		content = str.substr(0, content_length);
	}
}

void Request::print() const
{
	std::clog << "REQUEST LINE: ";
	start_line.print();
	std::clog << "\n\n";

	std::clog << "HEADERS: \n";
	for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		std::cout << "header-name: " << it->first << ", header-value: ";
		for (size_t i = 0; i < it->second.size(); ++i)
			std::cout << it->second[i] << ((i != it->second.size() - 1) ? "," : "\n");
	}
	std::clog << "\n\n";

	std::clog << "BODY: \n";
	if (content_length == 0)
		std::clog << "empty body\n";
	else
		std::clog << content << '\n';
}
