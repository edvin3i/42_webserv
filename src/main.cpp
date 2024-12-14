#include <iostream>
#include "../includes/signals/signals.hpp"
#include "../includes/server/TcpServer.hpp"
#include "../includes/config/ServerConfig.hpp"
#include "../includes/config/ConfigParser.hpp"
#include "../includes/logger/Logger.hpp"
#include "../includes/server/MasterServer.hpp"


#define ERR_NUM_ARGS "wrong number of arguments!"


int main(int argc, char **argv) {

	/*
	* Zero step: init signal handler
	*/
	setupSigHandler();

	/*
	 *	First - create a logger.
	 *	Details level: ERROR, INFO, DEBUG
	 *	Output mode: CONSOLE, LOGFILE, DUAL
	 */
	Logger logger(DEBUG, CONSOLE, "webserv.log");

	/*
	 * Second: Check args
	 */
	if (argc != 2) {
		logger.writeToLog(ERROR, ERR_NUM_ARGS);
		std::cout << ERR_NUM_ARGS << std::endl;
		std::cout << "USAGE: ./webserv <config.cfg>" << std::endl;
		return 1;
	}

	/*
	 * Third: Start the configuration file pareser
	 */
	std::string config_filename = argv[1];
	ConfigParser conf_parser(logger, config_filename);

	try {
		conf_parser.parse();
	}
	catch (std::exception &e) {
		std::cerr << BG_BRIGHT_RED << BRIGHT_WHITE << e.what() << RESET << std::endl;
		exit(EXIT_FAILURE);
	}

	//conf_parser.printConfig();
	logger.writeToLog(INFO, "Program Start Server!");

	/*
	 * Fourth: Start Master Server with main loop
	 */
	std::vector<ServerConfig> configs = conf_parser.getConfig();
	MasterServer masterServer(logger, configs);

	try {
		masterServer.run();
	}
	catch (std::exception &e) {
		std::cerr << BG_BRIGHT_RED << BRIGHT_WHITE << e.what() << RESET << std::endl;
		exit(EXIT_FAILURE);
	}


	return (0);
}