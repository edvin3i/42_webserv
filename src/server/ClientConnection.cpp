#include "../../includes/server/ClientConnection.hpp"
#include "../../includes/http/Request.hpp"
#include <iterator>

ClientConnection::ClientConnection(Logger & logger, int socketFD, const ServerConfig & config, Env& env)
									:	_logger(logger),
										_clientSocketFD(socketFD),
										_currentServerConfig(&config),
										_connectionState(READING),
										_writeOffset(0),
										_currentLocationConfig(NULL),
										_request(NULL),
										_response(NULL),
										_env(env),
										_keep_alive(true),
										_read_state(READ_REQUEST_LINE),
										_is_chunked(false),
										_is_content_length(false),
										_content_length(0),
										_timeout(false)
									{

}

ClientConnection::~ClientConnection() {
	if (_response)
		delete _response;
	if (_request)
		delete _request;

}


void ClientConnection::buildResponse() {

	_response = new Response(_logger, *_currentServerConfig, _currentLocationConfig, *_request, _env);
	_keep_alive = _response->keep_alive();
	std::stringstream ss;

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


void ClientConnection::_handle_request_line()
{
	size_t pos_delimiter = _readBuffer.find("\r\n");
	if (pos_delimiter != std::string::npos)
	{
		_request->setRequestLine(_readBuffer.substr(0, pos_delimiter));
		_readBuffer.erase(0, pos_delimiter + 2);
		_read_state = READ_HEADERS;
		_handle_headers();
	}
}

void ClientConnection::_handle_headers()
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
			return ;
		}
		fields = Utils::split(field_str, "\r\n");
		_request->setHeaders(fields);
		_readBuffer.erase(0, pos_delimiter + 4);
		_checkChunked();
		if (_is_chunked)
		{
			_read_state = READ_CONTENT_CHUNKED;
			_handle_chunked_content();
			return ;
		}
		_checkContentLength();
		if (_is_content_length)
		{
			_read_state = READ_CONTENT;
			_handle_content();
			return ;
		}
		if ((!_is_chunked || !_is_content_length) && !_readBuffer.empty())
		{
			_request->setError(STATUS_BAD_REQUEST);
		}
		_read_state = READ_FINISH;
		_handle_finish();
	}
}


void ClientConnection::_handle_content()
{
	if (_readBuffer.length() > _content_length)
	{
		_request->setError(STATUS_BAD_REQUEST);
		return ;
	}
	if (_readBuffer.length() == _content_length)
	{
		_request->setBody(_readBuffer.substr(0, _content_length), _content_length, false);
		_readBuffer.erase();
		_read_state = READ_FINISH;
		_handle_finish();
	}
}
void ClientConnection::_handle_chunked_content()
{
	size_t pos_delimiter = _readBuffer.find("\r\n\r\n");

	if (pos_delimiter != std::string::npos)
	{
		_request->setBody(_readBuffer.substr(0, pos_delimiter + 4), 0, true);
		_readBuffer.erase(0, pos_delimiter + 4);
		_read_state = READ_FINISH;
		_handle_finish();
	}
}

void ClientConnection::_handle_finish()
{
	if (!_readBuffer.empty())
		_request->setError(STATUS_BAD_REQUEST);
}

void ClientConnection::readData()
{
	char buffer[BUFFER_SIZE];
	ssize_t bytesReceived;
	
	if (_read_state == READ_CONTENT)
	{
		char *dynamic_buffer = NULL;
		size_t dynamic_buffer_size = _content_length - _readBuffer.length();

		try
		{
			dynamic_buffer = new char[dynamic_buffer_size];
		}
		catch (const std::bad_alloc& e)
		{
			_connectionState = CLOSING;
			return ;
		}
		bytesReceived = recv(_clientSocketFD, dynamic_buffer, dynamic_buffer_size, MSG_DONTWAIT);
		if (bytesReceived > 0)
			_readBuffer.append(dynamic_buffer, dynamic_buffer + bytesReceived);
		delete[] dynamic_buffer;
	}
	else
	{
		std::memset(buffer, 0, BUFFER_SIZE);
		bytesReceived = recv(_clientSocketFD, buffer, BUFFER_SIZE, MSG_DONTWAIT);
		if (bytesReceived > 0)
			_readBuffer.append(buffer, buffer + bytesReceived);

	}
	if (bytesReceived == 0)
	{
		if (_read_state != READ_FINISH)
			_connectionState = CLOSING;
		return ;
	}
	if (bytesReceived < 0 && _timeout)
	{
		_request->setError(STATUS_REQUEST_TIMEOUT);
	}
	switch (_read_state)
	{
		case READ_REQUEST_LINE:
		{
			_handle_request_line();
			break ;
		}
		case READ_HEADERS:
		{
			_handle_headers();
			break ;
		}
		case READ_CONTENT:
		{
			_handle_content();
			break ;
		}
		case READ_CONTENT_CHUNKED:
		{
			_handle_chunked_content();
			break ;
		}
		case READ_FINISH:
		{

			_handle_finish();
			break ;
		}
	}
	return ;
}

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
	                         bytesToSend, MSG_NOSIGNAL | MSG_DONTWAIT);

	if (bytesSent == -1) {
		return;
	}
	else if (bytesSent == 0) {
		_connectionState = CLOSING;
		return;
	}

	_writeOffset += bytesSent;

	// continue sending data until all data is sent
	if (_writeOffset >= _writeBuffer.size()) {

		std::ostringstream ss;
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

bool ClientConnection::isParsingFinish() const
{
	return (_read_state == READ_FINISH);
}

void ClientConnection::setTimeout(bool timeout)
{
	_timeout = timeout;
}