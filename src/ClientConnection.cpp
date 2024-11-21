#include "../includes/ClientConnection.hpp"

ClientConnection::ClientConnection(Logger & logger, const ServerConfig & config)
									:	_logger(logger),
										_serverConfig(config),
										//_currentLocationConfig(),
										_clientSocketFD()
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

}
