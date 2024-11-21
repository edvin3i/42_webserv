#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include "Logger.hpp"
#include "ServerConfig.hpp"


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
	std::string _srv_ip_address;
	int	_srv_port;
	int _srv_socket;
	int _srv_new_socket;
//		long _srv_incommingMessage;
	struct sockaddr_in _srv_socketAddress;
	unsigned int _srv_socketAddress_len;
	std::string _srv_serverMessage;

	int _startServer();
	void _closeServer();
	std::string _buildResponce();
	void _sendResponce();

	void _handleError(const std::string & err_message);

};






#endif
