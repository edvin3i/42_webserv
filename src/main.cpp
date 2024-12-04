#include <iostream>
#include <csignal>
#include "../includes/server/TcpServer.hpp"
#include "../includes/config/ServerConfig.hpp"
#include "../includes/config/ConfigParser.hpp"
#include "../includes/logger/Logger.hpp"
#include "../includes/server/MasterServer.hpp"

#define ERR_NUM_ARGS "wrong number of arguments!"


volatile sig_atomic_t g_sig = 0;


void sigIntHandler(int sig) {
	if (sig == SIGINT) {
		g_sig = 1;
	}
}


void setupSigHandler() {
	struct  sigaction sa = {};
	sa.sa_handler = sigIntHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		std::cerr << "Error with sigaction setup!" << std::endl;
		exit(EXIT_FAILURE);
	}
}

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
	conf_parser.parse();

	logger.writeToLog(INFO, "Program Start Server!");

	/*
	 * Fourth: Start Master Server with main loop
	 */
	std::vector<ServerConfig> configs = conf_parser.getConfig();
	MasterServer masterServer(logger, configs);
	masterServer.run();

	return (0);
}