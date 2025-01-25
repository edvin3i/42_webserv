#include "../../includes/server/ClientConnection.hpp"
#include "../../includes/http/Request.hpp"

#include <iterator>

ClientConnection::ClientConnection(Logger & logger, int socketFD, const ServerConfig & config, Env& env)
									:	_logger(logger),
										_clientSocketFD(socketFD),
										_currentServerConfig(&config),
										_connectionState(READING),
										_writeOffset(0),
										// _currentClientBodySize(0),
										_currentLocationConfig(NULL),
										_request(NULL),
										_env(env),
										_keep_alive(true),
										_read_state(READ_REQUEST_LINE),
										_is_chunked(false),
										_is_content_length(false),
										_content_length(0)
									{

}

ClientConnection::~ClientConnection() {

}


void ClientConnection::buildResponse() {

	_response = new Response(_logger, *_currentServerConfig, _currentLocationConfig, *_request, _env);
	_keep_alive = _response->keep_alive();
	std::stringstream ss;
	// ss << "RESPONSE:"<< '\n' << _response->toString();
	// _logger.writeToLog(DEBUG, ss.str());
	// ss.str("");

	std::string response_html = _response->toHtml();
	_writeBuffer.clear();
	_writeBuffer.resize(response_html.size());
	std::copy(response_html.begin(), response_html.end(), _writeBuffer.begin());
	_writeOffset = 0;

    _logger.writeToLog(DEBUG, "Response ready!");
	if (_response) {
		delete _response;
		_response = NULL;
	}
	if (_request) {
		delete _request;
		_request = NULL;
	}
}

void ClientConnection::_setRequestLineHeaders(const std::string& str)
{
	std::string request_line;
	std::string fields_str;
	std::vector<std::string> fields;
	size_t pos_delimiter;
	size_t pos_fields;

	pos_delimiter = str.find("\r\n");
	if (pos_delimiter == std::string::npos)
	{
		_request->setError(STATUS_BAD_REQUEST);
		return ;
	}
	request_line = str.substr(0, pos_delimiter);
	_request->setRequestLine(request_line);
	pos_fields = pos_delimiter + 2;
	if (pos_fields >= str.length() || Utils::is_whitespace(str[pos_fields]))
	{
		_request->setError(STATUS_BAD_REQUEST);
		return ;
	}
	fields_str = str.substr(pos_fields);
	fields = Utils::split(fields_str, "\r\n");
	_request->setHeaders(fields);
}

void ClientConnection::_checkContentLength()
{
	size_t nb_content_length = _request->getHeaders().count(Headers::getTypeStr(HEADER_CONTENT_LENGTH));

	switch (nb_content_length)
	{
		case 0:
			_content_length = 0;
			_is_content_length = false;
			break;
		case 1:
			try
			{
				Headers::const_iterator content_length_it = _request->getHeaders().find(Headers::getTypeStr(HEADER_CONTENT_LENGTH));
				_content_length = Utils::stoul(content_length_it->second.getValue());
				_is_content_length = true;
			}
			catch (const std::exception& e)
			{
				_request->setError(STATUS_BAD_REQUEST);
			}
			break ;
		default:
			_request->setError(STATUS_BAD_REQUEST);
		}
}

// void ClientConnection::_readRequestLineHeaders(std::string& readBuffer, std::string& content_begin)
// {
// 	char buffer[BUFFER_SIZE + 1];
// 	ssize_t bytesReceived;

// 	std::memset(buffer, 0, BUFFER_SIZE);
// 	while ((bytesReceived = recv(_clientSocketFD, buffer, BUFFER_SIZE, 0)) > 0)
// 	{
// 		char *pos_delimiter = std::strstr(buffer, "\r\n\r\n");
// 		if (pos_delimiter != NULL)
// 		{
// 			readBuffer.insert(readBuffer.end(), buffer, pos_delimiter);
// 			content_begin.insert(content_begin.end(), pos_delimiter + 4, buffer + bytesReceived);
// 			return ;
// 		}
// 		else
// 		{
//         	readBuffer.insert(readBuffer.end(), buffer, buffer + bytesReceived);
// 		}
//         std::memset(buffer, 0, BUFFER_SIZE);
// 	}
// 	if (bytesReceived == 0)
// 	{
// 		_request->setError(STATUS_BAD_REQUEST);
// 		_connectionState = CLOSING;
// 	}
// 	else if (bytesReceived < 0)
// 	{
// 		_request->setError(STATUS_INTERNAL_ERR);
// 		_connectionState = CLOSING;
// 	}
// }

// void ClientConnection::_readContent(std::string& readBuffer, size_t content_length)
// {
// 	char buffer[BUFFER_SIZE + 1];
// 	ssize_t bytesReceived;
// 	size_t totalBytesReceived = readBuffer.length();

// 	std::memset(buffer, 0, BUFFER_SIZE);
// 	while ((totalBytesReceived < content_length) && ((bytesReceived = recv(_clientSocketFD, buffer, BUFFER_SIZE, 0)) > 0))
// 	{
// 		readBuffer.insert(readBuffer.end(), buffer, buffer + bytesReceived);
// 		totalBytesReceived += bytesReceived;
// 		std::memset(buffer, 0, BUFFER_SIZE);
// 	}
// 	if (totalBytesReceived != content_length)
// 	{
// 		std::cerr << "ICI\n";
// 		_request->setError(STATUS_BAD_REQUEST);
// 	}
// 	if (bytesReceived < 0)
// 	{
// 		_request->setError(STATUS_INTERNAL_ERR);
// 	}
// }

void ClientConnection::_checkChunked()
{
	size_t nb_encoding = _request->getHeaders().count(Headers::getTypeStr(HEADER_TRANSFER_ENCODING));

	switch (nb_encoding)
	{
		case 0:
			_is_chunked = false;
			break ;
		case 1:
		{
			Headers::const_iterator transfer_encoding_it = _request->getHeaders().find(Headers::getTypeStr(HEADER_TRANSFER_ENCODING));
			if (transfer_encoding_it->second.getValue() == "chunked")
				_is_chunked = true;
			else
				_request->setError(STATUS_NOT_IMPLEMENTED);
			break ;
		}
		default:
			_request->setError(STATUS_NOT_IMPLEMENTED);
	}
}

// void ClientConnection::_readChunkedContent(std::string& readBuffer)
// {
// 	char buffer[BUFFER_SIZE + 1];
// 	ssize_t bytesReceived;

// 	std::memset(buffer, 0, BUFFER_SIZE);
// 	while (((bytesReceived = recv(_clientSocketFD, buffer, BUFFER_SIZE, 0)) > 0))
// 	{
// 		readBuffer.insert(readBuffer.end(), buffer, buffer + bytesReceived);
// 		if (std::strcmp(buffer + bytesReceived - 4, "\r\n\r\n") == 0)
// 			return ;
// 		std::memset(buffer, 0, BUFFER_SIZE);
// 	}
// 	if (bytesReceived == 0)
// 	{
// 		_request->setError(STATUS_BAD_REQUEST);
// 	}
// 	else if (bytesReceived < 0)
// 	{
// 		_request->setError(STATUS_INTERNAL_ERR);
// 	}
// }

ssize_t ClientConnection::readData()
{
	char buffer[BUFFER_SIZE + 1];
	ssize_t bytesReceived;

	std::memset(buffer, 0, BUFFER_SIZE + 1);
	bytesReceived = recv(_clientSocketFD, buffer, BUFFER_SIZE, 0);
	_readBuffer.append(buffer, buffer + bytesReceived);

	switch (_read_state)
	{
		case READ_REQUEST_LINE:
		{
			size_t pos_delimiter = _readBuffer.find("\r\n");
			if (pos_delimiter != std::string::npos)
			{
				_request->setRequestLine(_readBuffer.substr(0, pos_delimiter));
				_readBuffer.erase(0, pos_delimiter + 2);
				_read_state = READ_HEADERS;
			}
			break ;
		}
		case READ_HEADERS:
		{
			size_t pos_delimiter = _readBuffer.find("\r\n\r\n");
			if (pos_delimiter != std::string::npos)
			{
				std::string field_str;
				std::vector<std::string> fields;

				field_str = _readBuffer.substr(0, pos_delimiter);
				if (!field_str.empty() && Utils::is_whitespace(field_str[0]))
				{
					_request->setError(STATUS_BAD_REQUEST);
					break ;
				}
				fields = Utils::split(field_str, "\r\n");
				_request->setHeaders(fields);
				_readBuffer.erase(0, pos_delimiter + 4);
				_checkChunked();
				if (_is_chunked)
				{
					_read_state = READ_CONTENT_CHUNKED;
					break ;
				}
				_checkContentLength();
				if (_is_content_length)
				{
					_read_state = READ_CONTENT;
					break ;
				}
				if ((!_is_chunked || !_is_content_length) && !_readBuffer.empty())
				{
					_request->setError(STATUS_BAD_REQUEST);
				}
				_read_state = READ_FINISH;
			}
			break ;
		}
		case READ_CONTENT:
		{
			if (_readBuffer.length() > _content_length)
			{
				_request->setError(STATUS_BAD_REQUEST);
				break ;
			}
			if (_readBuffer.length() == _content_length)
			{
				_request->setBody(_readBuffer.substr(0, _content_length), _content_length, false);
				_read_state = READ_FINISH;
			}
			break ;
		}
		case READ_CONTENT_CHUNKED:
		{
			size_t pos_delimiter = _readBuffer.find("\r\n\r\n");

			if (pos_delimiter != std::string::npos)
			{
				_request->setBody(_readBuffer.substr(0, pos_delimiter + 4), 0, true);
				_readBuffer.erase(0, pos_delimiter + 4);
				if (!_readBuffer.empty())
				{
					_request->setError(STATUS_BAD_REQUEST);
					break ;
				}
				_read_state = READ_FINISH;
			}
			break ;
		}
		case READ_FINISH:
		{
			if (bytesReceived != 0)
				_request->setError(STATUS_BAD_REQUEST);
			break ;
		}
	}
	return (bytesReceived);
}

// void ClientConnection::readData()
// {
// 	std::string readBuffer, content_begin;
// 	size_t content_length = 0;
// 	bool is_content_length = false;
// 	bool is_chunked = false;
// 	_readRequestLineHeaders(readBuffer, content_begin);
// 	_setRequestLineHeaders(readBuffer);
// 	if (_request->error())
// 	{
// 		_connectionState = CLOSING;
// 		return ;
// 	}
// 	readBuffer.clear();
// 	readBuffer.append(content_begin);
// 	_checkChunked(is_chunked);
// 	if (_request->error())
// 	{
// 		_connectionState = CLOSING;
// 		return ;
// 	}
// 	if (is_chunked)
// 		_readChunkedContent(readBuffer);
// 	else
// 	{
// 		_checkContentLength(content_length, is_content_length);
// 		if (_request->error())
// 		{
// 			_connectionState = CLOSING;
// 			return ;
// 		}
// 		if (is_content_length)
// 			_readContent(readBuffer, content_length);
// 	}
// 	if (is_chunked || is_content_length)
// 		_request->setBody(readBuffer, content_length, is_chunked);
// 	_connectionState = CLOSING;
// }

// void ClientConnection::readData() {
// 	char buffer[BUFFER_SIZE + 1];
// 	size_t content_length = 0;
// 	std::memset(buffer, 0, BUFFER_SIZE + 1);
// 	ssize_t bytesReceived; //= recv(_clientSocketFD, buffer, BUFFER_SIZE, 0);
// 	bool delimiter_found = false;

//     while ((bytesReceived = recv(_clientSocketFD, buffer, BUFFER_SIZE, 0)) > 0)
// 	{
// 		char *pos_delimiter = std::strstr(buffer, "\r\n\r\n");
// 		if (!delimiter_found && pos_delimiter != NULL)
// 		{
// 			delimiter_found = true;
// 			_readBuffer.insert(_readBuffer.end(), buffer, buffer + (pos_delimiter - buffer + 2));
// 			_setRequestLineHeaders(_readBuffer);
// 			if (_request->error())
// 				return ;
// 			content_length = _getContentLength();
// 			if (_request->error())
// 				return ;
// 			_readBuffer.clear();
// 			_readBuffer.insert(_readBuffer.end(), buffer + (pos_delimiter - buffer + 2), buffer + bytesReceived);
// 		}
// 		else
// 		{
//         	_readBuffer.insert(_readBuffer.end(), buffer, buffer + bytesReceived);
// 		}
//         std::memset(buffer, 0, BUFFER_SIZE);
//     }
// 	if (bytesReceived == 0)
// 	{

// 	}
// 	// Add EAGAIN and EWOULDBLOCK checking
//     if ((bytesReceived < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
//         _connectionState = CLOSING;
//         return;
//     }

// 	std::stringstream ss;
// 	ss << "BUFFER Content:\n" << _readBuffer << "BUFFER Length:\n" << _readBuffer.length();
// 	_logger.writeToLog(DEBUG, ss.str());
// 	ss.str("");
// }


void ClientConnection::writeData() {
	if (_writeOffset >= _writeBuffer.size()) {
		_connectionState = CLOSING;
		return;
	}

	// send data by portions size of BUFFER_SIZE
	size_t remainingBytes = _writeBuffer.size() - _writeOffset;
	size_t bytesToSend = std::min(remainingBytes, static_cast<size_t>(BUFFER_SIZE));

	ssize_t bytesSent = send(_clientSocketFD,
	                         &_writeBuffer[_writeOffset],
	                         bytesToSend, MSG_NOSIGNAL);

	if (bytesSent < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			_logger.writeToLog(DEBUG, "socket not ready, try send next time...");
			return;
		}
		_logger.writeToLog(ERROR, "can not send the data to socket");
		_connectionState = CLOSING;
		return;
	}

	_writeOffset += bytesSent;

	std::ostringstream ss;
	ss << "===== Sent: " << bytesSent << " bytes to client. =====";
	_logger.writeToLog(DEBUG, ss.str());

	// continue sending data until all data is sent
	if (_writeOffset >= _writeBuffer.size()) {

		ss.str("");
		ss.clear();
		ss << "===== All data sent to the client! =====";
		_logger.writeToLog(DEBUG, ss.str());

		_connectionState = CLOSING;
	}
}


void ClientConnection::setState(ConnectionState state) {
	_connectionState = state;
}


ConnectionState ClientConnection::getState() const {
	return _connectionState;
}


bool ClientConnection::isReadyToWrite() {
	return !_writeBuffer.empty() \
			&& _writeOffset < _writeBuffer.size() \
			&& _connectionState == WRITING;
}

static size_t matching_prefix_depth(const std::string& location_path, const std::string& uri)
{
	std::vector<std::string> split_root = Utils::split_path(location_path);
	std::vector<std::string> split_uri = Utils::split_path(uri);
	bool uri_is_file = !(uri[uri.length() - 1] == '/');
	if (uri_is_file)
		split_uri.resize(split_uri.size() - 1);
	const size_t min_depth = std::min(split_root.size(), split_uri.size());
	size_t i = 0;

	if (location_path == "/")
		return (0);
	while (i < min_depth && (split_root[i] == split_uri[i]))
		i += 1;
	return (i);
}

// static bool is_localhost(const std::string& str)
// {
// 	return (str == "127.0.0.1" || str == "localhost");
// }

// void ClientConnection::select_server_config(const std::vector<ServerConfig>& configs)
// {
// 	const ServerConfig *config_ptr = NULL;
// 	bool host_equal, port_equal;

// 	for (size_t i = 0; i < configs.size(); ++i)
// 	{
// 		host_equal = false;
// 		port_equal = false;
// 		if (is_localhost(configs[i].host) && is_localhost(_request->getHost()))
// 			host_equal = true;
// 		else if (configs[i].host == _request->getHost())
// 			host_equal = true;
// 		if (configs[i].port == _request->getPort())
// 			port_equal = true;
// 		if (host_equal && port_equal)
// 			config_ptr = &configs[i];

// 	}
// 	if (config_ptr == NULL)
// 		_currentServerConfig = &configs[0];
// 	else
// 		_currentServerConfig = config_ptr;
// }

void ClientConnection::select_location()
{
	const std::map<std::string, LocationConfig> & locations = _currentServerConfig->locations;
	size_t max_depth = 0, depth;
	LocationConfig* location_tmp = NULL;

	// Add debug logging
	std::ostringstream ss;
	ss << "Request URI: " << _request->getStartLine().getUri().getPath() << "\n";
	ss << "Available locations:\n";
	for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
		ss << "- " << it->first << " (autoindex: " << (it->second.autoindex ? "on" : "off") << ")\n";
	}
	_logger.writeToLog(DEBUG, ss.str());


	if (locations.empty()) {
		_currentLocationConfig = NULL;
		_logger.writeToLog(DEBUG, "No locations found in server config");
		return;
	}

	for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		if (it->second.exact_match && it->first == _request->getStartLine().getUri().getPath())
		{
			_currentLocationConfig = const_cast<LocationConfig*>(&(it->second));
			return ;
		}
	}

	// First check if this is a root path request
	if (_request->getStartLine().getUri().getPath() == "/") {
		std::map<std::string, LocationConfig>::const_iterator root_it = locations.find("/");
		if (root_it != locations.end()) {
			_currentLocationConfig = const_cast<LocationConfig*>(&(root_it->second));
			_logger.writeToLog(DEBUG, "Using root location / for root path");
			return;
		}
	}

	// If not root path or no root location found, proceed with prefix matching
	for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin();
		 it != locations.end(); ++it)
	{
		if (it->first == "/")
			continue;
		if (it->second.exact_match)
			continue;

		depth = matching_prefix_depth(it->first, _request->getStartLine().getUri().getPath());
		ss.str("");
		ss << "Checking location " << it->first << ", depth: " << depth;
		_logger.writeToLog(DEBUG, ss.str());

		if (depth > 0 && depth > max_depth)
		{
			max_depth = depth;
			location_tmp = const_cast<LocationConfig*>(&(it->second));
			ss.str("");
			ss << "Found better match: " << it->first << " with depth " << depth;
			_logger.writeToLog(DEBUG, ss.str());
		}
	}

	if (!location_tmp) {
		std::map<std::string, LocationConfig>::const_iterator root_it = locations.find("/");
		if (root_it != locations.end()) {
			location_tmp = const_cast<LocationConfig*>(&(root_it->second));
			_logger.writeToLog(DEBUG, "Using root location / as fallback");
		} else {
			_logger.writeToLog(DEBUG, "No root location found");
		}
	}

	_currentLocationConfig = location_tmp;
	if (_currentLocationConfig) {
		ss.str("");
		ss << "Selected location config: " << (_currentLocationConfig->autoindex ? "autoindex on" : "autoindex off");
		_logger.writeToLog(DEBUG, ss.str());
	}
}

// void ClientConnection::setRequest()
// {
// 	_request = new Request(_logger, _readBuffer);
// }

void ClientConnection::initRequest()
{
	_request = new Request(_logger);
}

const Request* ClientConnection::getRequest() const
{
	return (_request);
}

bool ClientConnection::keep_alive() const
{
	return (_keep_alive);
}

ReadingState ClientConnection::getReadState() const
{
	return (_read_state);
}
