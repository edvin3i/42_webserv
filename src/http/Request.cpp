#include "../../includes/http/Request.hpp"

Request::Request(Logger& logger)
: Message<RequestLine>(), _logger(logger), _error_code(STATUS_OK), _error(false)
{}

Request::~Request() {}

Request::Request(const Request & other)
: Message<RequestLine>(other), _logger(other._logger), _error_code(other._error_code), _error(other._error)
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


void Request::_check_headers()
{
	if (headers.count(Headers::getTypeStr(HEADER_HOST)) != 1)
		throw (STATUS_BAD_REQUEST);
	const std::string& authority = start_line.getUri().getAuthority();
	if (!authority.empty())
	{
		Headers::iterator host_it = headers.find(Headers::getTypeStr(HEADER_HOST));
		host_it->second = authority;
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
	if (body.getContentLength() == 0)
		std::clog << "empty body\n";
	else
		std::clog << body.getContent() << '\n';
}


bool Request::error() const
{
	return (_error);
}

enum e_status_code Request::getErrorCode() const
{
	return (_error_code);
}

const RequestLine& Request::getStartLine() const
{
	return (start_line);
}

const Headers& Request::getHeaders() const
{
	return (headers);
}


const Body& Request::getBody() const
{
	return (body);
}

void Request::setRequestLine(const std::string& str)
{
	try
	{
		start_line = RequestLine(str);
	}
	catch (enum e_status_code code)
	{
		setError(code);
	}
	catch (...)
	{
		setError(STATUS_BAD_REQUEST);
	}
}

void Request::setHeaders(const std::vector<std::string>& fields)
{
	try
	{
		headers = Headers(fields);
		_check_headers();
	}
	catch (enum e_status_code code)
	{
		setError(code);
	}
	catch (...)
	{
		setError(STATUS_BAD_REQUEST);
	}
}

void Request::setBody(const std::string& str, size_t content_length, bool is_chunked)
{
	try
	{
		body = Body(str, content_length, headers, is_chunked);
	}
	catch (enum e_status_code code)
	{
		setError(code);
	}
	catch (...)
	{
		setError(STATUS_BAD_REQUEST);
	}
}

void Request::setError(enum e_status_code code)
{
	_error = true;
	_error_code = code;
}
