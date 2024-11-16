#include <iostream>
#include "../includes/TcpServer.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ConfigParser.hpp"




int main(int argc, char **argv) {
//	using namespace http;
//
//	TcpServer server = TcpServer("127.0.0.1", 8080);
//	server.startListen();

	if (argc != 2) {
		std::cout << "USAGE: ./webserv <config.cfg>" << std::endl;
		throw std::invalid_argument("ERROR: wrong number of arguments!");
	}
	std::string config_filename = argv[1];


	return (0);
}