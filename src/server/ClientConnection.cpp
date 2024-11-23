#include "../../includes/server/ClientConnection.hpp"

#include <iterator>

ClientConnection::ClientConnection(Logger & logger, int socketFD, const ServerConfig & config)
									:	_logger(logger),
										_clientSocketFD(socketFD),
										_serverConfig(config),
										_connectionState(READING)
									{
	(void) _serverConfig; // just to mute compile error

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
	_logger.writeToLog(DEBUG, "Response ready!");
	//ss.flush();
}


void ClientConnection::readData() {
	char buffer[BUFFER_SIZE] = {0};
	int bytesReceived = read(_clientSocketFD, buffer, BUFFER_SIZE);

	if (bytesReceived > 0) {
		_readBuffer.insert(_readBuffer.end(), buffer, buffer + bytesReceived);

		std::string str(_readBuffer.begin(), _readBuffer.end());
		_logger.writeToLog(DEBUG, "Receive a request from client:\n" + str);

		std::ostringstream ss;
		ss << "Read request from client. Len = " << bytesReceived << "\n";
		_logger.writeToLog(DEBUG, ss.str());
	}
	else {
		_connectionState = CLOSING;
	}
}


void ClientConnection::writeData() {
	unsigned long bytesSent;
	bytesSent = write(_clientSocketFD,
					  _responseMessage.c_str(),
					  _responseMessage.size());

	if (bytesSent == _responseMessage.size()) {
		std::ostringstream ss;
		ss << "--- Server response sent to client ---\n";
		ss << "\t\t\t Number of bytes sent: " << bytesSent << "\n";
		_logger.writeToLog(DEBUG, ss.str());
	}
	else {
		_logger.writeToLog(DEBUG, "Error sending response to client");
	}

	//_responseMessage.clear();
}


void ClientConnection::setState(ConnectionState state) {
	_connectionState = state;
}

ConnectionState ClientConnection::getState() const {
	return _connectionState;
}
