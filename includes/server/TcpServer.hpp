#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include "../logger/Logger.hpp"
#include "../config/ServerConfig.hpp"


class Logger;
class ServerConfig;


class TcpServer {
public:
	TcpServer(Logger & logger, const ServerConfig & config);
	TcpServer(const TcpServer & other);
	~TcpServer();
	int getSrvSocket() const;
	const ServerConfig &getConfig() const;

	void startListen();
	void startServer();
	int acceptConnection();


private:
	Logger &_logger;

	const ServerConfig &_config;
	std::string _IPAddress;
	int	_port;
	int _socket;
	struct sockaddr_in _socketAddress;
	unsigned int _socketAddressLen;

	bool _isValidIpAddress(const std::string & ip_address);
	void _resolveHostName(const std::string & hostname, std::string & ip_address);

	int _setupSocket();
	void _closeServer();

	void _handleError(const std::string & err_message);

};


#endif
