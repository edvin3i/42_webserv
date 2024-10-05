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
		std::string m_ip_address;
		int	m_port;
		int m_socket;
		int m_new_socket;
		long m_incommingMessage;
		struct sockaddr_in m_socketAddress;
		unsigned int m_socketAddress_len;
		std::string m_serverMessage;

		int startServer();
		void closeServer();
		void acceptConnection(int &new_socket);
		std::string buildResponce();
		void sendResponce();



	};

}




#endif
