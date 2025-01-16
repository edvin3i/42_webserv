#include <iostream>
#include "../includes/signals/signals.hpp"
#include "../includes/server/TcpServer.hpp"
#include "../includes/config/ServerConfig.hpp"
#include "../includes/config/ConfigParser.hpp"
#include "../includes/logger/Logger.hpp"
#include "../includes/server/MasterServer.hpp"
#include "../includes/http/Request.hpp"



#define INF_DEFAULT_CONF "config path not specified, using default configuration!"
#define INF_USE_CONF "using the config file: "
#define ERR_WRNG_NUM_ARGS "wrong number of args!"


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
	Logger &logger = Logger::get_logger(DEBUG, CONSOLE, "webserv.log");

	/*
	 * Second: Check args and set config path
	 */
	std::string config_filename;

	if (argc == 1) {
		logger.writeToLog(INFO, INF_DEFAULT_CONF);
		std::cout << INF_DEFAULT_CONF << std::endl;
		config_filename = "config/default.cfg";
	}
	else if (argc == 2) {
		config_filename = argv[1];
		logger.writeToLog(INFO, INF_USE_CONF + config_filename);
	}
	else {
		logger.writeToLog(ERROR, ERR_WRNG_NUM_ARGS);
		std::cerr << ERR_WRNG_NUM_ARGS << std::endl;
		std::cout << "USAGE: ./webserv <config.cfg>" << std::endl;;
		return 1;
	}

	/*
	 * Third: Start the configuration file pareser
	 */
	ConfigParser conf_parser(logger);

	try {
		conf_parser.init(config_filename);
		conf_parser.parse();
	}
	catch (std::exception &e) {
		std::cerr << BG_BRIGHT_RED << BRIGHT_WHITE << e.what() << RESET << std::endl;
		delete &logger;
		exit(EXIT_FAILURE);
	}

	// conf_parser.printConfig();
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
		delete &logger;
		exit(EXIT_FAILURE);
	}

	delete &logger;
	return (0);
}


// alternative main to test request class
// int main(void)
// {
// 	try
// 	{
// 	Request r("POST / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/1.1.0\r\nAccept: */*\r\nContent-Type: text/plain\r\nTransfer-Encoding: chunked\r\n\r\n7\r\nMozilla\r\n11\r\nDeveloper Network\r\n0\r\n\r\n");
// 	// Request r("GET / HTTP/1.1\r\nHost: localhost:8230\r\nUser-Agent: curl/7.81.0\r\n    Accept: */*\r\n\r\n");
// 	// Request r("GET / HTTP/1.1\r\nHost: localhost:8230\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\nTest:     a    ,    bb     ,       c c           \r\n\r\n");
// 	// Request r("GET / HTTP/1.1\r\nHost: localhost:8230\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\nContent-Length: 5, 5, 5, 6  \r\n\r\n12345");
// 	// Request r("GET / HTTP/1.1\r\n\r\n");
// 	r.print();
// 	}
// 	catch (int e)
// 	{
// 		std::cerr << "Error " << e << '\n';
// 	}
// }

