#include "../includes/TcpServer.hpp"


const int BUFFER_SIZE = 30720;

void log(const std::string &message) {
	std::cout << message << std::endl;
}

void exitWithError(const std::string &errorMessage) {
	log("Error: " + errorMessage);
	exit(1);
}

namespace http {

	http::TcpServer::TcpServer(std::string ip_address, int port)
	: _srv_ip_address(ip_address), _srv_port(port), _srv_socket(),
	  _srv_new_socket(), _srv_socketAddress(),
//	_srv_incommingMessage(),
	_srv_socketAddress_len(sizeof(_srv_socketAddress)),
	  _srv_serverMessage(_buildResponce()) {

		_srv_socketAddress.sin_family = AF_INET;
		_srv_socketAddress.sin_port = htons(_srv_port);
		_srv_socketAddress.sin_addr.s_addr = inet_addr(_srv_ip_address.c_str());

		if (_startServer() != 0) {
			std::ostringstream ss;
			ss << "Failed to start server with PORT: " << htons(_srv_socketAddress.sin_port);
			log(ss.str());
		}
	}

	http::TcpServer::~TcpServer() {
		_closeServer();
	}

	int http::TcpServer::_startServer() {
		_srv_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (_srv_socket < 0 ) {
			exitWithError("Cannot create socket!");
			return 1;
		}

		if (bind(_srv_socket, (sockaddr *)&_srv_socketAddress, _srv_socketAddress_len) < 0) {
			exitWithError("Cannot create socket to address!");
			return 1;
		}
		return 0;
	}

	void http::TcpServer::_closeServer() {
		close(_srv_socket);
		close(_srv_new_socket);
		exit(0);
	}

	void TcpServer::_acceptConnection(int &new_socket) {
		new_socket = accept(_srv_socket, (sockaddr *)&_srv_socketAddress, &_srv_socketAddress_len);
		if (new_socket < 0) {
			std::ostringstream ss;
			ss << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(_srv_socketAddress.sin_addr) << "; PORT: " << ntohs(_srv_socketAddress.sin_port);
			exitWithError(ss.str());
		}
	}

	std::string TcpServer::_buildResponce() {
		std::string htmlFile =  "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
		std::ostringstream ss;
		ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n" << htmlFile;


		return ss.str();
	}

	void TcpServer::_sendResponce() {
		unsigned long bytesSent;

		bytesSent = write(_srv_new_socket, _srv_serverMessage.c_str(), _srv_serverMessage.size());

		if (bytesSent == _srv_serverMessage.size()) {
			log("--- Server response sent to client ---\n\n");
		}
		else {
			log("Error sending responce to client");
		}

	}

	void TcpServer::startListen() {
		if (listen(_srv_socket, 20) < 0) {
			exitWithError("Socket listen failed!");
		}

		std::ostringstream ss;
		ss << "\n*** Listening on ADRESS: " << inet_ntoa(_srv_socketAddress.sin_addr) << " PORT: " << ntohs(_srv_socketAddress.sin_port) << " ***\n\n";
		log(ss.str());

		int bytesReceived;

		while (true) {
			log("=== Waiting new connection ===\n\n\n");
			_acceptConnection(_srv_new_socket);

			char buffer[BUFFER_SIZE] = {0};
			bytesReceived = read(_srv_new_socket, buffer, BUFFER_SIZE);
			if (bytesReceived < 0) {
				exitWithError("Failed to read data from client");
			}

			std::ostringstream ss;
			ss << "--- Received request from client ---\n\n";
			log(ss.str());

			_sendResponce();

			close(_srv_new_socket);
		}


	}
}

