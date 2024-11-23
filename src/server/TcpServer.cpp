#include "../../includes/server/TcpServer.hpp"


TcpServer::TcpServer(Logger & logger, const ServerConfig & config)
				: _logger(logger),
				  _config(config),
				  _IPAddress(),
				  _port(config.port),
				  _socket(),
				  _socketAddress(),
				  _socketAddressLen(sizeof(_socketAddress))
					{

	if (!_isValidIpAddress(_config.host)) {
		_resolveHostName(_config.host, _IPAddress);
	}
	else {
		_IPAddress = _config.host;
	}
	_socketAddress.sin_family = AF_INET;
	_socketAddress.sin_port = htons(_port);
	_socketAddress.sin_addr.s_addr = inet_addr(_IPAddress.c_str());
}


TcpServer::~TcpServer() {
	_closeServer();
}


TcpServer::TcpServer(const TcpServer & other)
		: _logger(other._logger),
		  _config(other.getConfig()),
		  _IPAddress(other._IPAddress),
		  _port(other._port),
		  _socket(other._socket),
		  _socketAddress(other._socketAddress),
		  _socketAddressLen(other._socketAddressLen) {

}


int TcpServer::_startServer() {
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	int flags = fcntl(_socket, F_GETFL, 0);
	fcntl(_socket, F_SETFL, flags | O_NONBLOCK);

	_logger.writeToLog(INFO, "Server started");

	if (_socket < 0 ) {
		_handleError("Cannot create socket!");
		return 1;
	}

	int bnd = bind(_socket, (sockaddr *)&_socketAddress, _socketAddressLen);
	if (bnd < 0) {
		_handleError("Cannot create socket to address!");
		return 1;
	}
	return 0;
}


void TcpServer::_closeServer() {
	close(_socket);
	exit(0);
}


int TcpServer::acceptConnection() {
	int new_socket = accept(
			_socket,
			(sockaddr *)&_socketAddress,
			&_socketAddressLen
					);

	if (new_socket < 0) {
		std::ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: " \
 			<< inet_ntoa(_socketAddress.sin_addr) \
 			<< "; PORT: " \
 			<< ntohs(_socketAddress.sin_port);
		_handleError(ss.str());
	}

	return new_socket;
}


void TcpServer::startListen() {
	int lstn = listen(_socket, 20);
	if (lstn < 0) {
		_handleError("Socket listen failed!");
	}

	std::ostringstream ss;
	ss << "\n*** Listening on ADRESS: " \
 		<< inet_ntoa(_socketAddress.sin_addr) \
 		<< " PORT: " << ntohs(_socketAddress.sin_port) \
 		<< " ***\n\n";
	_logger.writeToLog(INFO, ss.str());
	ss.flush();

}


void TcpServer::_handleError(const std::string & err_message) {
	std::string err_msg = err_message;
	_logger.writeToLog(ERROR, err_msg);
	throw std::runtime_error(err_msg);
}


void TcpServer::startServer() {
	if (_startServer() != 0) {
		std::ostringstream ss;
		ss << "Failed to startServer server with PORT: " \
			<< htons(_socketAddress.sin_port);
		_logger.writeToLog(ERROR, ss.str());
	}

}


int TcpServer::getSrvSocket() const {
	return _socket;
}


const ServerConfig &TcpServer::getConfig() const {
	return _config;
}


bool TcpServer::_isValidIpAddress(const std::string & ip_address) {
	struct sockaddr_in sa;
	return inet_pton(AF_INET, ip_address.c_str(), &(sa.sin_addr));
}

void TcpServer::_resolveHostName(const std::string &hostname,
	std::string &ip_address) {
	struct addrinfo hints = {}, *res;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(hostname.c_str(), NULL, &hints, &res);
	if (status != 0) {
		_logger.writeToLog(ERROR, "can not resolve the hostname.");
		return;
	}
	struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
	char ipStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ipv4->sin_addr), ipStr, INET_ADDRSTRLEN);
	ip_address = ipStr;

	freeaddrinfo(res);
}

