#include <iostream>
#include "../includes/TcpServer.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ConfigParser.hpp"




int main(int argc, char **argv) {
		std::cout << "ERROR: wrong number of arguments!" << std::endl;


	if (argc != 2) {
		std::cout << "USAGE: ./webserv <config.cfg>" << std::endl;
		return 1;
	}
	std::string config_filename = argv[1];
	ConfigParser conf_parser(config_filename);
	conf_parser.parse();
//	conf_parser.printConfig();

	std::vector<ServerConfig> servers_configs = conf_parser.getConfig();
	servers_configs[0].print_server_config();
	servers_configs[1].print_server_config();
	std::cout << servers_configs[1].locations[0].autoindex << std::endl;

	using namespace http;

	TcpServer server = TcpServer(servers_configs[1].host, servers_configs[1].port);
	server.startListen();



	return (0);
}