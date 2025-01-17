#include "../../includes/server/ClientConnection.hpp"
#include "../../includes/http/Request.hpp"

#include <iterator>

ClientConnection::ClientConnection(Logger & logger, int socketFD, const ServerConfig & config)
									:	_logger(logger),
										_clientSocketFD(socketFD),
										_currentServerConfig(NULL),
										_connectionState(READING),
										_writeOffset(0),
										_currentClientBodySize(0),
										_currentLocationConfig(NULL),
										_request(NULL)
									{
	(void) _currentServerConfig; // just to mute compile error
	(void) _currentClientBodySize;

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
    _writeBuffer.assign(response_html.begin(), response_html.end());
    _writeOffset = 0;

    _logger.writeToLog(DEBUG, "Response ready!");
	delete _response;
	delete _request;
}


void ClientConnection::readData() {
	char buffer[BUFFER_SIZE] = {0};
	size_t bytesReceived = recv(_clientSocketFD, buffer, BUFFER_SIZE, 0);

	if (bytesReceived > 0) {
		_readBuffer.insert(_readBuffer.end(), buffer, buffer + bytesReceived);

		// std::string str(_readBuffer.begin(), _readBuffer.end());
		// _logger.writeToLog(DEBUG, "Receive a request from client:\n" + str);

		// std::ostringstream ss;
		// ss << "Read request from client. Len = " << bytesReceived << "\n";
		// _logger.writeToLog(DEBUG, ss.str());
	}
	else {
		_connectionState = CLOSING;
	}
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


//https://nginx.org/en/docs/http/request_processing.html
void ClientConnection::select_server_config(std::vector<ServerConfig>& confs)
{
	if (!_request)
		throw (std::runtime_error("_request NULL"));

	const Headers::const_iterator host_it = _request->headers.find("Host");
	const std::string& host_request = host_it->second.getValue();
	bool host_found = false;

	for (std::vector<ServerConfig>::iterator it = confs.begin(); it != confs.end(); ++it)
	{
		if (it->host == host_request)
		{
			host_found = true;
			_currentServerConfig = &(*it);
			break ;
		}
	}
	if (!host_found)
		_currentServerConfig = &(*confs.begin());
}

static size_t matching_prefix_depth(const std::string& location_path, const std::string& uri)
{
	std::vector<std::string> split_root = Utils::split(location_path, "/");
	std::vector<std::string> split_uri = Utils::split(uri, "/");
	const size_t min_depth = std::min(split_root.size(), split_uri.size());
	size_t i = 0;

	if (location_path == "/" || uri == "/")
		return (1);
	while (i < min_depth && (split_root[i] == split_uri[i]))
		i += 1;
	return (i);
}

void ClientConnection::select_location()
{
	std::vector<LocationConfig>& locations = _currentServerConfig->locations;
	size_t max_depth = 0, depth;
	LocationConfig* location_tmp = NULL;

	if (locations.empty())
		_currentLocationConfig = NULL;
	for (std::vector<LocationConfig>::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		depth = matching_prefix_depth(it->path, _request->start_line.getUri().getPath());
		if (depth > max_depth)
		{
			max_depth = depth;
			location_tmp = &(*it);
		}
	}
	_currentLocationConfig = location_tmp;
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
