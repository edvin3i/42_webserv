#include "../../includes/http/Request.hpp"


Request::Request(const std::string & str)
: Message<RequestLine>(), _error(false), _error_code(STATUS_OK)
{
	try
	{
		_parse(str);
	}
	catch (enum e_status_code error_code)
	{
		_error = true;
		_error_code = error_code;
	}
}

Request::~Request() {}

Request::Request(const Request & other)
: Message<RequestLine>(other)
{}

Request& Request::operator=(const Request & other)
{
	if (this != &other)
	{
		Message<RequestLine>::operator=(other);
	}
	return (*this);
}

void Request::_split_request(std::string str, std::string & request_line, std::vector<std::string> & header_lines, std::string & body)
{
	const std::string delimiter = "\r\n";
	size_t pos_start = 0, pos_end, delim_len = 2;
	std::string header_line;

	pos_end = str.find(delimiter, pos_start);
	request_line = str.substr(pos_start, pos_end - pos_start);
	pos_start = pos_end + delim_len;
	if (Utils::is_whitespace(str[pos_start]))
		throw (400); // reject message as invalid
	while (1)
	{
		pos_end = str.find(delimiter, pos_start);
		if (pos_end == std::string::npos)
			throw (STATUS_BAD_REQUEST);
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

void Request::_parse(const std::string & str)
{
	std::string request_line, body;
	std::vector<std::string> header_lines;

	_split_request(str, request_line, header_lines, body);
	start_line = RequestLine(request_line);
	_parse_headers(header_lines);
	_check_headers();
	_parse_body(body);
}

void Request::_parse_headers(std::vector<std::string> & header_lines)
{
	std::string header_line_trim;
	for (size_t i = 0; i < header_lines.size(); ++i)
	{
		header_line_trim = _str_trim(header_lines[i]);
		_parse_header(header_line_trim);
	}
}

static bool is_delimiter(char c)
{
	const std::string delimiters = "(),/:;<=>?@[\\]{}";

	return (delimiters.find(c) != std::string::npos);
}

void Request::_handle_quoted_str(const std::string& str, size_t& i, std::string& element) const
{
	if (!element.empty())
		throw (400);
	i += 1;
	while (1)
	{
		if (i == str.length())
			throw (400);
		if (str[i] == '\"')
		{
			if (str.length() == (i + 1) || is_delimiter(str[i + 1]) || Utils::is_whitespace(str[i + 1]))
			{
				i += 1;
				return ;
			}
			else
				throw (400);
		}
		element.push_back(str[i]);
		i += 1;
	}
}

void Request::_parse_field_value(const std::string & str, const std::string & field_name)
{
	std::string element;
	size_t i = 0;
	size_t nb_non_empty_element = 0;

	while (i < str.length())
	{
		switch (str[i])
		{
			case '\r': case '\n': case '\0':
				throw (400);
			case ' ': case '\t':
				if (element.empty())
				{
					while (Utils::is_whitespace(str[i]))
						i += 1;
				}
				else
				{
					size_t nb_whitespace = 0;
					while (Utils::is_whitespace(str[i]))
					{
						nb_whitespace += 1;
						i += 1;
					}
					if (!is_delimiter(str[i]))
						element.append(nb_whitespace, ' ');
				}
				break ;
			case '\"':
				_handle_quoted_str(str, i, element);
				break ;
			case ',':
				if (!element.empty())
					nb_non_empty_element += 1;
				headers.insert(std::pair<std::string, std::string>(field_name, element));
				element.clear();
				i += 1;
				break ;
			case '(':
			{
				int depth = 0;
				while (true)
				{
					if (i == str.length())
						throw (STATUS_BAD_REQUEST);
					if (str[i] == '(')
						depth++;
					else if (str[i] == ')')
						depth--;
					if (str[i] == ')' && (depth == 0))
					{
						i += 1;
						break ;
					}
					if (depth < 0)
						throw (STATUS_BAD_REQUEST);
					i += 1;
				}
				break ;
			}
			default:
				element.push_back(str[i]);
				i += 1;
				break ;
		}
	}
	if (!element.empty())
		nb_non_empty_element += 1;
	headers.insert(Field(field_name, FieldValue(element)));
	if (nb_non_empty_element == 0)
		throw (STATUS_BAD_REQUEST);
}

std::string Request::_str_trim(const std::string &str) const
{
	size_t str_start, str_end;

	str_start = str.find_first_not_of(Utils::whitespace);
	str_end = str.find_last_not_of(Utils::whitespace);
	return (str.substr(str_start, str_end - str_start + 1));
}

void Request::_parse_header(const std::string &str)
{
	std::string field_name, field_value, field_value_trim;
	size_t colon_pos;

	if (str.length() > max_header_length)
		throw (STATUS_BAD_REQUEST);
	colon_pos = str.find(':');
	field_name = str.substr(0, colon_pos);
	if (field_name.empty() || Utils::is_whitespace(field_name[field_name.length() - 1]))
		throw (400);
	field_value = str.substr(colon_pos + 1);
	field_value_trim = _str_trim(field_value);
	_parse_field_value(field_value_trim, field_name);
}

void Request::_check_headers()
{
	//respond with 400 when request message contains more than one Host header field line or a Host header field with an invalid field value
	if (headers.count("Host") != 1)
		throw (STATUS_BAD_REQUEST);
	const std::string& authority = start_line.getUri().getAuthority();
	if (!authority.empty())
	{
		Headers::iterator host_it = headers.find("Host");
		host_it->second = authority;
	}
}

void Request::_decode_chunked(const std::string & str)
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
	headers.insert(Field("Content-Length", ss_content_length.str()));
	// headers.insert(std::pair<std::string, std::vector<std::string> >("Content-Length", std::vector<std::string>(1, ss_content_length.str())));
	content_length = length;
	std::pair<Headers::iterator, Headers::iterator> transfer_encoding_key = headers.equal_range("Transfer-Encoding");
	for (Headers::iterator it = transfer_encoding_key.first; it != transfer_encoding_key.second; ++it)
	{
		if (it->second.getValue() == "chunked")
		{
			it->second.setValue("");
			break ;
		}
	}
}

static void _read_size_t(const std::string & str, size_t & n)
{
	std::stringstream sstream(str);
	sstream >> n;
}

void Request::_parse_body(const std::string & str)
{
	std::pair<Headers::iterator, Headers::iterator> transfer_encoding_it = headers.equal_range("Transfer-Encoding");
	std::pair<Headers::iterator, Headers::iterator> content_length_it = headers.equal_range("Content-Length");
	size_t nb_encoding = headers.count("Transfer-Encoding");
	size_t nb_content_length = headers.count("Content-Length");

	if (str.empty())
		return ;
	if (nb_encoding > 0 && nb_content_length > 0)
	{
		throw (STATUS_BAD_REQUEST);
		//close connection after responding
	}
	else if (nb_encoding > 0)
	{
		if (nb_encoding == 1 && transfer_encoding_it.first->second.getValue() == "chunked")
			_decode_chunked(str);
		else
			throw (STATUS_NOT_IMPLEMENTED);
	}
	else if(nb_content_length > 0)
	{
		switch (headers.count("Content-Length"))
		{
			case '0':
				throw (STATUS_LENGTH_REQUIRED);
			case '1':
				_read_size_t(headers.find("Content-Length")->second.getValue(), content_length);
				break ;
			default:
				size_t tmp;
				Headers::iterator it = content_length_it.first;

				_read_size_t(it->second.getValue(), content_length);
				it++;
				while (it != content_length_it.second)
				{
					_read_size_t(it->second.getValue(), tmp);
					if (tmp != content_length)
						throw (STATUS_LENGTH_REQUIRED);
					it++;
				}
				break ;
		}
		content = str.substr(0, content_length);
	}
	// {
	// 	std::string boundary = headers.find("Content-Type")->second.getParameters().find("boundary")->second;
	// 	size_t count = 0;
	// 	size_t old_pos = 0;
	// 	size_t pos;

	// 	while ((pos = str.find(boundary, old_pos)) != std::string::npos)
	// 	{
	// 		count += 1;
	// 		old_pos = pos + boundary.length();
	// 	}
	// 	std::cout << "NB BOUNDARY: " << count << '\n';
	// 	std::cout << content_length << '\n';
	// 	std::cout << content.substr(content_length - 100, std::string::npos);
	// }
}

void Request::print() const
{
	std::clog << "REQUEST LINE: ";
	start_line.print();
	std::clog << "\n\n";

	// std::clog << "HEADERS: \n";
	// for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it)
	// {
	// 	std::cout << "header-name: " << it->first << ", header-value: ";
	// 	for (size_t i = 0; i < it->second.size(); ++i)
	// 		std::cout << it->second[i] << ((i != it->second.size() - 1) ? "," : "\n");
	// }
	// std::clog << "\n\n";

	std::clog << "HEADERS: \n";
	for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		std::cout << "header-name: " << it->first << ", header-value: " << it->second.getValue();
		const Parameters& parameters = it->second.getParameters();
		if (parameters.size() > 0)
		{
			std::cout << ", parameters: ";
			for (Parameters::const_iterator jt = parameters.begin(); jt != parameters.end(); ++jt)
			{
				std::cout << jt->first << '=' << jt->second << ' ';
			}
		}
		std::cout << '\n';
	}
	std::clog << "\n\n";

	std::clog << "BODY: \n";
	if (content_length == 0)
		std::clog << "empty body\n";
	else
		std::clog << content << '\n';
}


bool Request::error() const
{
	return (_error);
}

enum e_status_code Request::getErrorCode() const
{
	return (_error_code);
}
