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

void log(const std::string &message);
void exitWithError(std::string &message);


class Logger;



class TcpServer {
public:
	TcpServer(Logger & logger, const std::string & ip_address, int port);
	TcpServer(const TcpServer & other);
	~TcpServer();
	void startListen();
	void start();


private:
	Logger &_logger;
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
	void _acceptConnection(int & new_socket);
	std::string _buildResponce();
	void _sendResponce();

	void _handleError(const std::string & err_message);

};






#endif
