#include <iostream>
#include <sys/stat.h>
#include "../includes/signals/signals.hpp"
#include "../includes/server/TcpServer.hpp"
#include "../includes/config/ServerConfig.hpp"
#include "../includes/config/ConfigParser.hpp"
#include "../includes/logger/Logger.hpp"
#include "../includes/server/MasterServer.hpp"
#include "../includes/http/Request.hpp"



#define INF_DEFAULT_CONF "looking for default config file in the config directory: "
#define INF_USE_CONF "using the config file: "
#define ERR_WRNG_NUM_ARGS "wrong number of args!"


int main(int argc, char **argv, char **env) {

	/*
	* Zero step: init signal handler
	*/
	setupSigHandler();


   /*
	 *	First - create a logger.
	 *	Details level: ERROR, INFO, DEBUG
	 *	Output mode: CONSOLE, LOGFILE, DUAL
	 */
	Logger &logger = Logger::get_logger(INFO, CONSOLE, "webserv.log");

	/*
	 * Second: Check args and set config path
	 */
	std::string config_filename;

	if (argc == 2) {
		struct stat st;

		std::string config_path = argv[1];
		if (stat(config_path.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
			
			config_filename = config_path;
			if (config_filename[config_filename.size() - 1] != '/') {
				config_filename += "/";
			}
			config_filename += "default.cfg";
			logger.writeToLog(INFO, INF_DEFAULT_CONF + config_path);
		}
		else {
			config_filename = config_path;
			logger.writeToLog(INFO, INF_USE_CONF + config_filename);
		}
		
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
		return (EXIT_FAILURE);
	}

	logger.writeToLog(INFO, "Program Start Server!");

	/*
	 * Fourth: Start Master Server with main loop
	 */
	std::vector<ServerConfig> configs = conf_parser.getConfig();

	try {
		MasterServer masterServer(logger, configs, env);
		masterServer.run();
	}
	catch (const Response::ChildProcessException& e)
	{
		delete &logger;
		return (EXIT_FAILURE);
	}
	catch (const std::exception &e) {
		std::cerr << BG_BRIGHT_RED << BRIGHT_WHITE << e.what() << RESET << std::endl;
		delete &logger;
		return (EXIT_FAILURE);
	}
	catch (...)
	{
		delete &logger;
		return (EXIT_FAILURE);
	}

	delete &logger;
	return (EXIT_SUCCESS);
}


