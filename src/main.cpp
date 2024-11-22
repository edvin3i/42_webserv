#include <iostream>
#include "../includes/server/TcpServer.hpp"
#include "../includes/config/ServerConfig.hpp"
#include "../includes/config/ConfigParser.hpp"
#include "../includes/logger/Logger.hpp"
#include "../includes/server/MasterServer.hpp"

#define ERR_NUM_ARGS "wrong number of arguments!"


int main(int argc, char **argv) {

	Logger logger(DEBUG, CONSOLE, "webserv.log");
//	logger.printCurrentDateTime();

	if (argc != 2) {
		logger.writeToLog(ERROR, ERR_NUM_ARGS);
		std::cout << ERR_NUM_ARGS << std::endl;
		std::cout << "USAGE: ./webserv <config.cfg>" << std::endl;
		return 1;
	}

	std::string config_filename = argv[1];
	ConfigParser conf_parser(logger, config_filename);
	conf_parser.parse();
	// conf_parser.printConfig();

//	servers_configs[0].print_server_config();
//	servers_configs[1].print_server_config();
//	std::cout << servers_configs[1].locations[0].autoindex << std::endl;
//	std::cout << servers_configs[1].locations[0].autoindex << std::endl;


	logger.writeToLog(INFO, "Program Start Server!");

	std::vector<ServerConfig> configs = conf_parser.getConfig();
	MasterServer masterServer(logger, configs);
	masterServer.run();

	return (0);
}