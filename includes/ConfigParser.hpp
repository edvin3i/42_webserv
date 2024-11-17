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
#include "Logger.hpp"

class ServerConfig;
class LocationConfig;

class ConfigParser {

public:
	ConfigParser(std::string &conf_filename, Logger & logger);
	~ConfigParser();
	void parse();
	std::vector<ServerConfig> getConfig();
	void printConfig();



private:
	std::vector<ServerConfig > _servers;
	std::string _config_path;
	std::ifstream _config_file;
	std::string _current_line;
	size_t _line_number;
	Logger &_logger;

	void _parseServerBlock(ServerConfig & server);
	void _parseLocationBlock(LocationConfig & location, ServerConfig & server);
	void _trim(std::string & rawString);
	bool _startsWith(const std::string & str, const std::string & prefix);
	std::vector<std::string> _tokenize(const std::string &rawString);
	size_t _convertDataSize(const std::string &dataSize);
	bool _convertOnOff(const std::string & switchState);
	void _handleError(const std::string & err_message);



};


#endif
