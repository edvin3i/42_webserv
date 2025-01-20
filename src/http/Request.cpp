#include "../../includes/http/Request.hpp"

Request::Request(Logger & logger, const std::string & str)
: _logger(logger), Message<RequestLine>(), _error(false), _error_code(STATUS_OK)
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
	catch (...)
	{
		_error = true;
		_error_code = STATUS_BAD_REQUEST;
	}
}

Request::~Request() {}

Request::Request(const Request & other)
: Message<RequestLine>(other), _logger(other._logger), _error(other._error), _error_code(other._error_code)
{}

Request& Request::operator=(const Request & other)
{
	if (this != &other)
	{
		Message<RequestLine>::operator=(other);
		_error = other._error;
		_error_code = other._error_code;
	}
	return (*this);
}

void Request::_split_request(const std::string& str, std::string & request_line, std::vector<std::string> & header_lines, std::string & body)
{
	const std::string delimiter = "\r\n";
	size_t pos_start = 0, pos_end;
	std::string header_line;

	pos_end = str.find(delimiter, pos_start);
	if (pos_end == std::string::npos)
		throw (STATUS_BAD_REQUEST);
	request_line = str.substr(pos_start, pos_end - pos_start);
	pos_start = pos_end + delimiter.length();
	if (pos_start >= str.length())
		throw (STATUS_BAD_REQUEST);
	if (Utils::is_whitespace(str[pos_start]))
		throw (STATUS_BAD_REQUEST); // reject message as invalid
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
		pos_start = pos_end + delimiter.length();
	}
}

void Request::_parse(const std::string & str)
{
	std::string request_line, body;
	std::vector<std::string> fields;

	_split_request(str, request_line, fields, body);
	start_line = RequestLine(request_line);
	headers = Headers(fields);
	_check_headers();
	_parse_body(body);
	_handle_multipart();
}

void Request::_check_headers()
{
	//respond with 400 when request message contains more than one Host header field line or a Host header field with an invalid field value
	if (headers.count(Headers::getTypeStr(HEADER_HOST)) != 1)
		throw (STATUS_BAD_REQUEST);
	const std::string& authority = start_line.getUri().getAuthority();
	if (!authority.empty())
	{
		Headers::iterator host_it = headers.find(Headers::getTypeStr(HEADER_HOST));
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
	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), ss_content_length.str()));
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
	std::pair<Headers::iterator, Headers::iterator> content_length_it = headers.equal_range(Headers::getTypeStr(HEADER_CONTENT_LENGTH));
	size_t nb_encoding = headers.count("Transfer-Encoding");
	size_t nb_content_length = headers.count(Headers::getTypeStr(HEADER_CONTENT_LENGTH));

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
		switch (headers.count(Headers::getTypeStr(HEADER_CONTENT_LENGTH)))
		{
			case '0':
				throw (STATUS_LENGTH_REQUIRED);
			case '1':
				_read_size_t(headers.find(Headers::getTypeStr(HEADER_CONTENT_LENGTH))->second.getValue(), content_length);
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

void Request::_skip_newline(size_t& i)
{
	if (content.compare(i, 2, "\r\n") != 0)
		throw (STATUS_BAD_REQUEST);
	i += 2;
}

void Request::_handle_multipart()
{

	size_t nb_content_type = headers.count(Headers::getTypeStr(HEADER_CONTENT_TYPE));
	static size_t test = 0;

	if (nb_content_type == 0)
		return ;
	if (nb_content_type > 1)
		throw (STATUS_BAD_REQUEST);

	Headers::const_iterator content_type_it = headers.find(Headers::getTypeStr(HEADER_CONTENT_TYPE));
	const std::string content_type_value = content_type_it->second.getValue();
	const std::string multipart = "multipart";

	if (content_type_value.compare(0, multipart.length(), multipart) != 0)
		return ;
	const Parameters& parameters = content_type_it->second.getParameters();
	std::string boundary;
	if (!parameters.count("boundary"))
		throw (STATUS_BAD_REQUEST);
	boundary = parameters.at("boundary");

	const std::string delimiter = "--" + boundary;
	size_t i;
	size_t pos;
	if (content.compare(0, delimiter.length(), delimiter) != 0)
		throw (STATUS_BAD_REQUEST);
	i = delimiter.length();
	_skip_newline(i);
	while (i < content_length)
	{
		pos = content.find(delimiter, i);
		if (pos == std::string::npos)
			throw (STATUS_BAD_REQUEST);
		if (content.compare(pos + delimiter.length(), 2, "--") == 0)
		{
			size_t tmp = pos + delimiter.length() + 2;
			_skip_newline(tmp);
			_multipart.push_back(BodyPart(content.substr(i, pos - i - 2)));
			return ;
		}
		else
		{
			_multipart.push_back(BodyPart(content.substr(i, pos - i)));
		}
		i = pos + delimiter.length();
		_skip_newline(i);
	}
}

const std::vector<BodyPart>& Request::getMultipart() const
{
	return (_multipart);
}

const RequestLine& Request::getStartLine() const
{
	return (start_line);
}

const Headers& Request::getHeaders() const
{
	return (headers);
}

const std::string& Request::getContent() const
{
	return (content);
}

size_t Request::getContentLength() const
{
	return (content_length);
}
