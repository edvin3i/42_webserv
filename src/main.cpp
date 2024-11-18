#include <iostream>
#include "../includes/TcpServer.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/ConfigParser.hpp"
#include "../includes/Logger.hpp"
#include "../includes/MasterServer.hpp"

#define ERR_NUM_ARGS "ERROR: wrong number of arguments!"


int main(int argc, char **argv) {

	Logger logger;
//	logger.printCurrentDateTime();

	if (argc != 2) {
		logger.writeToLog(ERR_NUM_ARGS);
		std::cout << ERR_NUM_ARGS << std::endl;
		std::cout << "USAGE: ./webserv <config.cfg>" << std::endl;
		return 1;
	}
	std::string config_filename = argv[1];
	ConfigParser conf_parser(config_filename, logger);
	conf_parser.parse();
//	conf_parser.printConfig();

	std::vector<ServerConfig> servers_configs = conf_parser.getConfig();
//	servers_configs[0].print_server_config();
//	servers_configs[1].print_server_config();
//	std::cout << servers_configs[1].locations[0].autoindex << std::endl;
//	std::cout << servers_configs[1].locations[0].autoindex << std::endl;


	logger.writeToLog("Program start!");

	std::vector<ServerConfig> configs = conf_parser.getConfig();
	MasterServer masterServer(configs, logger);


	return (0);
}