#include "../includes/ClientConnection.hpp"

ClientConnection::ClientConnection(Logger & logger, int socketFD, const ServerConfig & config)
									:	_logger(logger),
										_clientSocketFD(socketFD),
										_serverConfig(config),
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
	_logger.writeToLog("Response ready!");
	//ss.flush();
}

void ClientConnection::sendResponse() {
	unsigned long bytesSent;
	bytesSent = write(_clientSocketFD,
					  _responseMessage.c_str(),
					  _responseMessage.size());

	if (bytesSent == _responseMessage.size()) {
		_logger.writeToLog("--- Server response sent to client ---\n\n");
	}
	else {
		_logger.writeToLog("Error sending response to client");
	}

	//_responseMessage.clear();
}

void ClientConnection::readData() {
	char buffer[BUFFER_SIZE] = {0};
	int bytesReceived = read(_clientSocketFD, buffer, BUFFER_SIZE);

	// std::ostringstream ss;
	// ss << buffer << "\n";
	// std::cout << ss.str();

	if (bytesReceived > 0) {
		_readBuffer.insert(_readBuffer.end(), buffer, buffer + bytesReceived);
		// maybe need to write to log counter of bytes here

		std::ostringstream ss;
		ss << "Read request from client. Len = " << bytesReceived << "\n";
		std::cout << ss.str();
		_logger.writeToLog(ss.str());
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
