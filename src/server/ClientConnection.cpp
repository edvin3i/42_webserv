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
    // std::string indexPath = _serverConfig.root + _serverConfig.index;
    // std::ifstream htmlFile(indexPath.c_str());

	// std::stringstream buffer;
	// std::ostringstream ss;


    // if (!htmlFile) {
    //     // throw(STATUS_NOT_FOUND); // call 404 page
    // 	// buffer << htmlFile.rdbuf();
    // 	ss << "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: \r\n\r\n";
    // 	ss << "<html>";
	// 	ss << "<head><title>404 Not Found</title></head>";
	// 	ss << "<body>";
    // 	ss << "<center><h1>404 Not Found</h1></center>";
	// 	ss << "<hr><center>WebServ42/0.00.01</center>";
	// 	ss << "</body>";
	// 	ss << "</html>";
	// 	ss << "\r\n\r\n";
    // }
	// else {
	// 	buffer << htmlFile.rdbuf();
	// 	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
	// 		<< buffer.str().size() << "\r\n\r\n" << buffer.str();
	// }

	// std::string responce = ss.str(); // set string instead temporary ss.str object
	_response = new Response(*_request, *_currentServerConfig, _currentLocationConfig);
	std::cout << "RESPONSE:"<< '\n' << _response->toString();
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

    if (bytesReceived == 0 || (bytesReceived < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
        _connectionState = CLOSING;
        return;
    }

	// if (bytesReceived > 0) {
	// 	_readBuffer.insert(_readBuffer.end(), buffer, buffer + bytesReceived);

	// 	// std::string str(_readBuffer.begin(), _readBuffer.end());
	// 	// _logger.writeToLog(DEBUG, "Receive a request from client:\n" + str);

	// 	// std::ostringstream ss;
	// 	// ss << "Read request from client. Len = " << bytesReceived << "\n";
	// 	// _logger.writeToLog(DEBUG, ss.str());
	// }
	// else {
	// 	_connectionState = CLOSING;
	// }
	// RequestParser httpParser;

	// httpParser.parse(_readBuffer);
	std::clog << "buffer:\n" << _readBuffer;
}


void ClientConnection::writeData() {
	if (_writeOffset >= _writeBuffer.size()) {
		_connectionState = CLOSING;
		return;
	}

	ssize_t bytesSent = send(_clientSocketFD,
	                         &_writeBuffer[_writeOffset],
	                         _writeBuffer.size() - _writeOffset, 0);

	if (bytesSent < 0) {
		_logger.writeToLog(ERROR, "can not send the data to socket");
		_connectionState = CLOSING;
		return;
	}

	_writeOffset += bytesSent;

	std::ostringstream ss;
	ss << "===== Sent: " << bytesSent << " bytes to client. =====" << std::endl;
	_logger.writeToLog(DEBUG, ss.str());
	ss.str("");
	ss.clear();

	if (_writeOffset >= _writeBuffer.size()) {
		ss << "===== All data sent to the client! =====" << std::endl;
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
	_request = new Request(_readBuffer);
}

// void ClientConnection::setLocationConfig()
// {
// 	const std::vector<LocationConfig>& locations = _serverConfig.locations;
// 	size_t length = 0;
// 	std::string path;
// 	size_t location_index = 0;
// 	bool location_found = false;
// 	const std::string& _uri = _request->start_line.getUri();

// 	for (size_t i = 0; i < locations.size(); ++i)
// 	{
// 		path = locations[i].path;
// 		if (path.length() > _uri.length())
// 			continue;
// 		size_t tmp_length = 0;
// 		for (size_t j = 0; j < _uri.length() && path[j] == _uri[j]; ++j)
// 			tmp_length++;
// 		if (tmp_length > length)
// 		{
// 			location_found = true;
// 			length = tmp_length;
// 			location_index = i;
// 		}
// 	}
// 	if (location_found)
// 		_currentLocationConfig = new LocationConfig[location_index];
// }
