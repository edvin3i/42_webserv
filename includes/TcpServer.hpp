#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <sys/socket.h>
#include <string>

void log(const std::string &message);
void exitWithError(std::string &message);

namespace http {

	class TcpServer {
	public:
		TcpServer();
		~TcpServer();

		int startServer(std::string ip_addr, int port);
		void closeServer();

	private:
		std::string m_ip_address;
		int	m_port;
		int m_socket;
		int m_new_socket;
		long m_incommingMessage;
		struct sockaddr_in m_socketAddress;
		unsigned int m_socketAddress_len;
		std::string m_serverMessage;



	};

}




#endif
