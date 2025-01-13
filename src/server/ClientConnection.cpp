#include "../../includes/server/ClientConnection.hpp"
#include "../../includes/http/Request.hpp"

#include <iterator>

ClientConnection::ClientConnection(Logger & logger, int socketFD, const ServerConfig & config)
									:	_logger(logger),
										_clientSocketFD(socketFD),
										_serverConfig(config),
										_connectionState(READING),
										_writeOffset(0),
										_currentClientBodySize(0),
										_currentLocationConfig(NULL),
										_request(NULL)
									{
	(void) _serverConfig; // just to mute compile error
	(void) _currentClientBodySize;

}

ClientConnection::~ClientConnection() {

}


void ClientConnection::buildResponse() {
    std::string indexPath = _serverConfig.root + _serverConfig.index;
    std::ifstream htmlFile(indexPath.c_str());

	std::stringstream buffer;
	std::ostringstream ss;


    if (!htmlFile) {
        // throw(STATUS_NOT_FOUND); // call 404 page
    	// buffer << htmlFile.rdbuf();
    	ss << "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: \r\n\r\n";
    	ss << "<html>";
		ss << "<head><title>404 Not Found</title></head>";
		ss << "<body>";
    	ss << "<center><h1>404 Not Found</h1></center>";
		ss << "<hr><center>WebServ42/0.00.01</center>";
		ss << "</body>";
		ss << "</html>";
		ss << "\r\n\r\n";
    }
	else {
		buffer << htmlFile.rdbuf();
		ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
			<< buffer.str().size() << "\r\n\r\n" << buffer.str();
	}

	std::string responce = ss.str(); // set string instead temporary ss.str object
	_writeBuffer.clear();
    _writeBuffer.assign(responce.begin(), responce.end());
    _writeOffset = 0;

    _logger.writeToLog(DEBUG, "Response ready!");
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
	Request request(_readBuffer);
	request.print();


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


void ClientConnection::setLocationConfig()
{
	const std::vector<LocationConfig>& locations = _serverConfig.locations;
	size_t length = 0;
	std::string path;
	size_t location_index = 0;
	bool location_found = false;
	const std::string& _uri = _request->start_line.getUri();

	for (size_t i = 0; i < locations.size(); ++i)
	{
		path = locations[i].path;
		if (path.length() > _uri.length())
			continue;
		size_t tmp_length = 0;
		for (size_t j = 0; j < _uri.length() && path[j] == _uri[j]; ++j)
			tmp_length++;
		if (tmp_length > length)
		{
			location_found = true;
			length = tmp_length;
			location_index = i;
		}
	}
	if (location_found)
		_currentLocationConfig = new LocationConfig[location_index];
}
