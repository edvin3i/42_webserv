#include <iostream>
#include "../includes/server/TcpServer.hpp"
#include "../includes/config/ServerConfig.hpp"
#include "../includes/config/ConfigParser.hpp"
#include "../includes/logger/Logger.hpp"
#include "../includes/server/MasterServer.hpp"
#include "../includes/http/Request.hpp"


#define ERR_NUM_ARGS "wrong number of arguments!"


int main(int argc, char **argv) {


	/*
	 *	First - create a logger.
	 *	Details level: ERROR, INFO, DEBUG
	 *	Output mode: CONSOLE, LOGFILE, DUAL
	 */

	Logger logger(DEBUG, CONSOLE, "webserv.log");
//	logger.printCurrentDateTime();

	if (argc != 2) {
		logger.writeToLog(ERROR, ERR_NUM_ARGS);
		std::cout << ERR_NUM_ARGS << std::endl;
		std::cout << "USAGE: ./webserv <config.cfg>" << std::endl;
		return 1;
	}

	/*
	 * Start the configuration file pareser
	 */
	std::string config_filename = argv[1];
	ConfigParser conf_parser(logger, config_filename);
	conf_parser.parse();

	logger.writeToLog(INFO, "Program Start Server!");

	/*
	 * Start Master Server with main loop
	 */
	std::vector<ServerConfig> configs = conf_parser.getConfig();
	MasterServer masterServer(logger, configs);
	masterServer.run();

	return (0);
}


// alternative main to test request class
// int main(void)
// {
// 	// Request r("POST / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/1.1.0\r\nAccept: */*\r\nContent-Type: text/plain\r\nTransfer-Encoding: chunked\r\n\r\n7\r\nMozilla\r\n11\r\nDeveloper Network\r\n0\r\n\r\n");
// 	Request r("GET / HTTP/1.1\r\nHost: localhost:8230\r\nUser-Agent: curl/7.81.0\r\nAccept: */*\r\n\r\n");
// 	r.print();
// }
