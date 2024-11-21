#include "../includes/ClientConnection.hpp"

ClientConnection::ClientConnection(Logger & logger, const ServerConfig & config)
									:	_logger(logger),
										_serverConfig(config),
										//_currentLocationConfig(),
										_clientSocketFD(),
										_connectionState(READING)
									{

}

ClientConnection::~ClientConnection() {

}

void ClientConnection::buildResponse() {
	std::string htmlFile =  "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from my WEBSERV42  :) </p></body></html>";
	std::ostringstream ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " \
		<< htmlFile.size() \
		<< "\n\n" \
		<< htmlFile;
	_responseMessage = ss.str();
	ss.flush();
}

void ClientConnection::sendResponse() {
	unsigned long bytesSent;
	bytesSent = write(_newServerSocket,
					  _responseMessage.c_str(),
					  _responseMessage.size());

	if (bytesSent == _responseMessage.size()) {
		_logger.writeToLog("--- Server response sent to client ---\n\n");
	}
	else {
		_logger.writeToLog("Error sending response to client");
	}
}

void ClientConnection::readData() {
	char buffer[BUFFER_SIZE] = {0};
	int bytesReceived = read(_clientSocketFD, buffer, BUFFER_SIZE);

	if (bytesReceived > 0) {
		_readBuffer.insert(_readBuffer.end(), buffer, buffer + bytesReceived);
		// maybe need to write to log counter of bytes here
	}
	else {
		_connectionState = CLOSING;
	}
}

void ClientConnection::setState(ConnectionState state) {
	_connectionState = state;
}

ConnectionState ClientConnection::getState() const {
	return _connectionState;
}

void ClientConnection::closeConnection() {
// just empty yet
}
