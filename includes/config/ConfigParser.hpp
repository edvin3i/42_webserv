#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <fstream>
#include "ServerConfig.hpp"
#include "../../includes/logger/Logger.hpp"

class ServerConfig;
class LocationConfig;

class ConfigParser {

public:
	ConfigParser(Logger & logger, std::string & conf_filename);
	~ConfigParser();
	void parse();
	std::vector<ServerConfig> getConfig();
	void printConfig();



private:
	Logger &_logger;
	std::vector<ServerConfig > _servers;
	std::string _config_path;
	std::ifstream _config_file;
	std::string _current_line;
	size_t _line_number;

	void _parseServerBlock(ServerConfig & server);
	void _parseLocationBlock(LocationConfig &location);
	void _trim(std::string & rawString);
	bool _startsWith(const std::string & str, const std::string & prefix);
	std::vector<std::string> _tokenize(const std::string &rawString);
	size_t _convertDataSize(const std::string &dataSize);
	bool _convertOnOff(const std::string & switchState);
	void _handleError(const std::string & err_message);

};


#endif
