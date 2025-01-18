#include "../../includes/server/ClientConnection.hpp"
#include "../../includes/http/Request.hpp"

#include <iterator>

ClientConnection::ClientConnection(Logger & logger, int socketFD, const ServerConfig & config)
									:	_logger(logger),
										_clientSocketFD(socketFD),
										_currentServerConfig(&config),
										_connectionState(READING),
										_writeOffset(0),
										_currentClientBodySize(0),
										_currentLocationConfig(NULL),
										_request(NULL)
									{

}

ClientConnection::~ClientConnection() {

}


void ClientConnection::buildResponse() {
   
	_response = new Response(_logger, *_currentServerConfig, _currentLocationConfig, *_request);
	
	std::stringstream ss;
	ss << "RESPONSE:"<< '\n' << _response->toString();
	_logger.writeToLog(DEBUG, ss.str());
	ss.str("");
	
	std::string response_html = _response->toHtml();
	_writeBuffer.clear();
	_writeBuffer.resize(response_html.size());
	std::copy(response_html.begin(), response_html.end(), _writeBuffer.begin());
	_writeOffset = 0;

    _logger.writeToLog(DEBUG, "Response ready!");
	delete _response;
	delete _request;
}


void ClientConnection::readData() {
	char buffer[BUFFER_SIZE] = {0};
	ssize_t bytesReceived; //= recv(_clientSocketFD, buffer, BUFFER_SIZE, 0);

    while ((bytesReceived = recv(_clientSocketFD, buffer, BUFFER_SIZE, 0)) > 0) {
        _readBuffer.insert(_readBuffer.end(), buffer, buffer + bytesReceived);
        
  
        std::memset(buffer, 0, BUFFER_SIZE);
   
    }

	// Add EAGAIN and EWOULDBLOCK checking
    if (bytesReceived == 0 || (bytesReceived < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
        _connectionState = CLOSING;
        return;
    }

	std::stringstream ss;
	ss << "BUFFER Content:\n" << _readBuffer;
	_logger.writeToLog(DEBUG, ss.str());
	ss.str("");
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
	                         bytesToSend, 0);

	if (bytesSent < 0) {
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


static size_t matching_prefix_depth(const std::string& location_root, const std::string& uri)
{
	// Special case for root location
	if (location_root == "/")
		return (0);

	// If URI is just "/", it should only match root location
	if (uri == "/")
		return (0);

	std::vector<std::string> split_root = Utils::split(location_root, "/");
	std::vector<std::string> split_uri = Utils::split(uri, "/");
	const size_t min_depth = std::min(split_root.size(), split_uri.size());
	size_t i = 0;

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
	ss << "Request URI: " << _request->start_line.getUri().getPath() << "\n";
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

	// First check if this is a root path request
	if (_request->start_line.getUri().getPath() == "/") {
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


		depth = matching_prefix_depth(it->first, _request->start_line.getUri().getPath());
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

void ClientConnection::setRequest()
{
	_request = new Request(_logger, _readBuffer);
}
