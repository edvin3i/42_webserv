#include "../../includes/http/Body.hpp"

Body::Body()
: _content(), _content_length(0), _is_multipart(false), _multipart()
{}

Body::Body(const std::string& str)
: _content(str), _content_length(str.length()), _is_multipart(false), _multipart()
{}

// Body::Body(const std::string& str, const Headers& headers)
// : _is_content_length(false), _is_chunked(false), _content(), _content_length(0), _is_multipart(false), _multipart()
// {
// 	_parse_body(str, headers);
// 	_handle_multipart(headers);
// }

Body::Body(const std::string& content, size_t content_length, const Headers& headers, bool is_chunked)
: _content(content), _content_length(content_length), _is_multipart(false), _multipart()
{
	if (is_chunked)
		_decode_chunked(content);
	_handle_multipart(headers);
}

Body::~Body()
{}

Body::Body(const Body& other)
: _content(other._content),
  _content_length(other._content_length),
  _is_multipart(other._is_multipart),
  _multipart(other._multipart)
{}

Body& Body::operator=(const Body& other)
{
	if (this != &other)
	{
		_content = other._content;
		_content_length = other._content_length;
		_is_multipart = other._is_multipart;
		_multipart = other._multipart;
	}
	return (*this);
}

// void Body::_setContentLength(const Headers& headers)
// {
// 	Headers::const_iterator content_length_it = headers.find(Headers::getTypeStr(HEADER_CONTENT_LENGTH));
// 	size_t nb_content_length = headers.count(Headers::getTypeStr(HEADER_CONTENT_LENGTH));

// 	switch (nb_content_length)
// 	{
// 		case 0:
// 			_content_length = 0;
// 			_is_content_length = false;
// 			break;
// 		case 1:
// 			_content_length = Utils::stoul(content_length_it->second.getValue());
// 			_is_content_length = true;
// 			break ;
// 		default:
// 			throw (STATUS_BAD_REQUEST);
// 		}
// }

void Body::_checkContentLength(const std::string& content)
{
	if (_content_length == 0 && content.length() != 0)
		throw (STATUS_LENGTH_REQUIRED);
	if (_content_length != 0 && _content_length != content.length())
		throw (STATUS_BAD_REQUEST);
}

// void Body::_check_chunked(const Headers& headers)
// {
// 	Headers::const_iterator transfer_encoding_it = headers.find(Headers::getTypeStr(HEADER_TRANSFER_ENCODING));
// 	size_t nb_encoding = headers.count(Headers::getTypeStr(HEADER_TRANSFER_ENCODING));

// 	switch (nb_encoding)
// 	{
// 		case 0:
// 			_is_chunked = false;
// 			break ;
// 		case 1:
// 			if (transfer_encoding_it->second.getValue() == "chunked")
// 				_is_chunked = true;
// 			else
// 				throw (STATUS_NOT_IMPLEMENTED);
// 			break ;
// 		default:
// 			throw (STATUS_NOT_IMPLEMENTED);
// 	}
// }

// void Body::_parse_body(const std::string& str, const Headers& headers)
// {
// 	try
// 	{
// 		_setContentLength(headers);
// 	}
// 	catch (const std::exception& e)
// 	{
// 		throw (STATUS_BAD_REQUEST);
// 	}
// 	_checkContentLength(str);
// 	if (str.empty())
// 	{
// 		return ;
// 	}
// 	_check_chunked(headers);

// 	if (_is_chunked)
// 		_decode_chunked(str);
// 	else if (_is_content_length)
// 	{
// 		_content = str;
// 	}
// }

void Body::_decode_chunked(const std::string& str)
{
	size_t length = 0, chunk_size;
	char *chunk_data;
	std::istringstream sstream(str);

	sstream >> std::hex >> chunk_size;
	if (!sstream)
		throw (STATUS_BAD_REQUEST);
	sstream.ignore(1, '\r');
	sstream.ignore(1, '\n');
	while (chunk_size > 0)
	{
		chunk_data = new char[chunk_size + 1];
		sstream.read(chunk_data, chunk_size);
		chunk_data[chunk_size] = '\0';
		sstream.ignore(1, '\r');
		sstream.ignore(1, '\n');
		_content.append(chunk_data);
		delete[] chunk_data;
		length += chunk_size;
		sstream >> std::hex >> chunk_size;
		if (!sstream)
			throw (STATUS_BAD_REQUEST);
		sstream.ignore(1, '\r');
		sstream.ignore(1, '\n');
	}
	std::stringstream ss_content_length;
	ss_content_length << length;
	_content_length = length;
}

void Body::_skip_newline(const std::string& str, size_t& i)
{
	if (str.compare(i, 2, "\r\n") != 0)
		throw (STATUS_BAD_REQUEST);
	i += 2;
}

void Body::_handle_multipart(const Headers& headers)
{
	size_t nb_content_type = headers.count(Headers::getTypeStr(HEADER_CONTENT_TYPE));

	if (nb_content_type == 0)
	{
		_is_multipart = false;
		return ;
	}
	if (nb_content_type > 1)
		throw (STATUS_BAD_REQUEST);

	Headers::const_iterator content_type_it = headers.find(Headers::getTypeStr(HEADER_CONTENT_TYPE));
	const std::string content_type_value = content_type_it->second.getValue();
	const std::string multipart = "multipart";

	if (content_type_value.compare(0, multipart.length(), multipart) != 0)
		return ;
	_is_multipart = true;
	const Parameters& parameters = content_type_it->second.getParameters();
	std::string boundary;
	if (!parameters.count("boundary"))
		throw (STATUS_BAD_REQUEST);
	boundary = parameters.at("boundary");

	const std::string delimiter = "--" + boundary;
	size_t i;
	size_t pos;
	if (_content.compare(0, delimiter.length(), delimiter) != 0)
		throw (STATUS_BAD_REQUEST);
	i = delimiter.length();
	_skip_newline(_content, i);
	while (i < _content_length)
	{
		pos = _content.find(delimiter, i);
		if (pos == std::string::npos)
			throw (STATUS_BAD_REQUEST);
		if (_content.compare(pos + delimiter.length(), 2, "--") == 0)
		{
			size_t tmp = pos + delimiter.length() + 2;
			_skip_newline(_content, tmp);
			_multipart.push_back(BodyPart(_content.substr(i, pos - i - 2)));
			return ;
		}
		else
		{
			_multipart.push_back(BodyPart(_content.substr(i, pos - i)));
		}
		i = pos + delimiter.length();
		_skip_newline(_content, i);
	}
}


const std::string& Body::getContent() const
{
	return (_content);
}
size_t Body::getContentLength() const
{
	return (_content_length);
}

bool Body::is_mutlipart() const
{
	return (_is_multipart);
}

const std::vector<BodyPart>& Body::getMultipart() const
{
	return (_multipart);
}

void Body::setContent(const std::string& content)
{
	_content = content;
}

void Body::setContentLength(size_t n)
{
	_content_length = n;
}

void Body::addContent(const std::string& str)
{
	_content.append(str);
}
