#include "../includes/TcpServer.hpp"


//const int BUFFER_SIZE = 8192;


TcpServer::TcpServer(Logger & logger, const ServerConfig & config)
				:	_logger(logger),
					_config(config),
					_srv_ip_address(_config.host),
					_srv_port(config.port),
					_srv_socket(),
					_srv_new_socket(),
					_srv_socketAddress(),
//					_srv_incommingMessage(),
					_srv_socketAddress_len(sizeof(_srv_socketAddress)),
					_srv_serverMessage(_buildResponce()) {

	_srv_socketAddress.sin_family = AF_INET;
	_srv_socketAddress.sin_port = htons(_srv_port);
	_srv_socketAddress.sin_addr.s_addr = inet_addr(_srv_ip_address.c_str());
}


TcpServer::~TcpServer() {
	_closeServer();
}


int TcpServer::_startServer() {
	_srv_socket = socket(AF_INET, SOCK_STREAM, 0);
	int flags = fcntl(_srv_socket, F_GETFL, 0);
	fcntl(_srv_socket, F_SETFL, flags | O_NONBLOCK);

	std::cout << "Server started" << std::endl;

	if (_srv_socket < 0 ) {
		_handleError("Cannot create socket!");
		return 1;
	}
	int bnd = bind(_srv_socket, (sockaddr *)&_srv_socketAddress, _srv_socketAddress_len);
	if (bnd < 0) {
		_handleError("Cannot create socket to address!");
		return 1;
	}
	return 0;
}


void TcpServer::_closeServer() {
	close(_srv_socket);
	close(_srv_new_socket);
	exit(0);
}


int TcpServer::acceptConnection() {
	int new_socket = accept(
					_srv_socket,
					(sockaddr *)&_srv_socketAddress,
					&_srv_socketAddress_len
					);

	if (new_socket < 0) {
		std::ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: " \
			<< inet_ntoa(_srv_socketAddress.sin_addr) \
			<< "; PORT: " \
			<< ntohs(_srv_socketAddress.sin_port);
		_handleError(ss.str());
	}

	return new_socket;
}


std::string TcpServer::_buildResponce() {
	std::string htmlFile =  "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from my WEBSERV42  :) </p></body></html>";
	std::ostringstream ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " \
		<< htmlFile.size() \
		<< "\n\n" \
		<< htmlFile;
	return ss.str();
}


void TcpServer::_sendResponce() {
	unsigned long bytesSent;
	bytesSent = write(_srv_new_socket, _srv_serverMessage.c_str(), _srv_serverMessage.size());

	if (bytesSent == _srv_serverMessage.size()) {
		_logger.writeToLog("--- Server response sent to client ---\n\n");
	}
	else {
		_logger.writeToLog("Error sending responce to client");
	}
}


void TcpServer::startListen() {
	int lstn = listen(_srv_socket, 20);
	if (lstn < 0) {
		_handleError("Socket listen failed!");
	}

	std::ostringstream ss;
	ss << "\n*** Listening on ADRESS: " \
		<< inet_ntoa(_srv_socketAddress.sin_addr) \
		<< " PORT: " << ntohs(_srv_socketAddress.sin_port) \
		<< " ***\n\n";
	_logger.writeToLog(ss.str());
	ss.flush();


	/*int bytesReceived;
	while (true) {
		_logger.writeToLog("=== Waiting new connection ===\n\n\n");
		_acceptConnection(_srv_new_socket);
		char buffer[BUFFER_SIZE] = {0};
		bytesReceived = read(_srv_new_socket, buffer, BUFFER_SIZE);
		if (bytesReceived < 0) {
			_handleError("Failed to read data from client");
		}

		std::ostringstream ss;
		ss << "--- Received request from client ---\n\n";
		_logger.writeToLog(ss.str());

		_sendResponce();
		close(_srv_new_socket);
	}*/
}


void TcpServer::_handleError(const std::string & err_message) {
	std::string err_msg = err_message;
	_logger.writeToLog(err_msg);
	throw std::runtime_error(err_msg);
}


TcpServer::TcpServer(const TcpServer & other)
		: _logger(other._logger),
		  _config(other.getConfig()),
		  _srv_ip_address(other._srv_ip_address),
		  _srv_port(other._srv_port),
		  _srv_socket(),
		  _srv_new_socket(),
		  _srv_socketAddress(other._srv_socketAddress),
		  _srv_socketAddress_len(other._srv_socketAddress_len),
		  _srv_serverMessage(other._srv_serverMessage) {

}


void TcpServer::startServer() {
	if (_startServer() != 0) {
		std::ostringstream ss;
		ss << "Failed to startServer server with PORT: " \
			<< htons(_srv_socketAddress.sin_port);
		_logger.writeToLog(ss.str());
	}

}

int TcpServer::getSrvSocket() const {
	return _srv_socket;
}

const ServerConfig &TcpServer::getConfig() const {
	return _config;
}
