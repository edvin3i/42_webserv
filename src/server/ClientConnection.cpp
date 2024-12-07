#include "../../includes/server/ClientConnection.hpp"

#include <iterator>

ClientConnection::ClientConnection(Logger & logger, int socketFD, const ServerConfig & config)
									:	_logger(logger),
										_clientSocketFD(socketFD),
										_serverConfig(config),
										_connectionState(READING),
										_writeOffset(0),
										_currentClientBodySize(0)
									{
	(void) _serverConfig; // just to mute compile error
	(void) _currentClientBodySize;

}

ClientConnection::~ClientConnection() {

}


void ClientConnection::buildResponse() {
	std::string htmlFile =  "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from my WebServ_42  :) </p></body></html>";
	std::ostringstream ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " \
		<< htmlFile.size() \
		<< "\r\n\r\n" \
		<< htmlFile;
	std::string response = ss.str();
	_writeBuffer.assign(response.begin(), response.end());
	_writeOffset = 0;

	_logger.writeToLog(DEBUG, "Response ready!");
	ss.flush();
}


void ClientConnection::readData() {
	char buffer[BUFFER_SIZE] = {0};
	size_t bytesReceived = recv(_clientSocketFD, buffer, BUFFER_SIZE, 0);

	if (bytesReceived > 0) {
		_readBuffer.insert(_readBuffer.end(), buffer, buffer + bytesReceived);

		// std::string str(_readBuffer.begin(), _readBuffer.end());
		// _logger.writeToLog(DEBUG, "Receive a request from client:\n" + str);

		std::ostringstream ss;
		ss << "Read request from client. Len = " << bytesReceived << "\n";
		_logger.writeToLog(DEBUG, ss.str());
	}
	else {
		_connectionState = CLOSING;
	}
	RequestParser httpParser;

	httpParser.parse(_readBuffer);

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
