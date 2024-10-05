#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>

void log(const std::string &message);
void exitWithError(std::string &message);


namespace http {

	class TcpServer {
	public:
		TcpServer(std::string ip_address, int port);
		~TcpServer();

		void startListen();


	private:
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
		void _acceptConnection(int &new_socket);
		std::string _buildResponce();
		void _sendResponce();

	};

}




#endif
