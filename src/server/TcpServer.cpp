#include "../../includes/server/TcpServer.hpp"


TcpServer::TcpServer(Logger & logger, const ServerConfig & config)
				: _logger(logger),
				  _config(config),
				  _serverIPAddress(_config.host),
				  _serverPort(config.port),
				  _serverSocket(),
				  _serverSocketAddress(),
				  _serverSocketAddressLen(sizeof(_serverSocketAddress))
					{

						_serverSocketAddress.sin_family = AF_INET;
						_serverSocketAddress.sin_port = htons(_serverPort);
						_serverSocketAddress.sin_addr.s_addr = inet_addr(_serverIPAddress.c_str());
}


TcpServer::~TcpServer() {
	_closeServer();
}


int TcpServer::_startServer() {
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	int flags = fcntl(_serverSocket, F_GETFL, 0);
	fcntl(_serverSocket, F_SETFL, flags | O_NONBLOCK);

	std::cout << "Server started" << std::endl;

	if (_serverSocket < 0 ) {
		_handleError("Cannot create socket!");
		return 1;
	}

	// std::ostringstream ss;
	// ss << "Server Socket: " << _serverSocket << "\n";
	// ss << "Server Socket Addr: " << (sockaddr *)&_serverSocketAddress << "\n";
	// ss << "Server Socket Addr Len: " << _serverSocketAddressLen << std::endl;
	//
	// std::cout << ss.str();

	int bnd = bind(_serverSocket, (sockaddr *)&_serverSocketAddress, _serverSocketAddressLen);
	if (bnd < 0) {
		_handleError("Cannot create socket to address!");
		return 1;
	}
	return 0;
}


void TcpServer::_closeServer() {
	close(_serverSocket);
	exit(0);
}


int TcpServer::acceptConnection() {
	int new_socket = accept(
			_serverSocket,
			(sockaddr *)&_serverSocketAddress,
			&_serverSocketAddressLen
					);

	if (new_socket < 0) {
		std::ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: " \
 			<< inet_ntoa(_serverSocketAddress.sin_addr) \
 			<< "; PORT: " \
 			<< ntohs(_serverSocketAddress.sin_port);
		_handleError(ss.str());
	}

	return new_socket;
}


void TcpServer::startListen() {
	int lstn = listen(_serverSocket, 20);
	if (lstn < 0) {
		_handleError("Socket listen failed!");
	}

	std::ostringstream ss;
	ss << "\n*** Listening on ADRESS: " \
 		<< inet_ntoa(_serverSocketAddress.sin_addr) \
 		<< " PORT: " << ntohs(_serverSocketAddress.sin_port) \
 		<< " ***\n\n";
	_logger.writeToLog(INFO, ss.str());
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
	_logger.writeToLog(ERROR, err_msg);
	throw std::runtime_error(err_msg);
}


TcpServer::TcpServer(const TcpServer & other)
		: _logger(other._logger),
		  _config(other.getConfig()),
		  _serverIPAddress(other._serverIPAddress),
		  _serverPort(other._serverPort),
		  _serverSocket(other._serverSocket),
		  _serverSocketAddress(other._serverSocketAddress),
		  _serverSocketAddressLen(other._serverSocketAddressLen) {

}


void TcpServer::startServer() {
	if (_startServer() != 0) {
		std::ostringstream ss;
		ss << "Failed to startServer server with PORT: " \
			<< htons(_serverSocketAddress.sin_port);
		_logger.writeToLog(ERROR, ss.str());
	}

}

int TcpServer::getSrvSocket() const {
	return _serverSocket;
}

const ServerConfig &TcpServer::getConfig() const {
	return _config;
}
