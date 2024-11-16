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

class ServerConfig;
class LocationConfig;

class ConfigParser {

public:
	ConfigParser(std::string & conf_filename);
	~ConfigParser();
	void parse();


private:
	std::vector<ServerConfig > _servers;
	std::string _config_path;
	std::ifstream _config_file;
	std::string _current_line;
	size_t _line_number;
	void _parseServerBlock(ServerConfig & server);
	void _parseLocationBlock(LocationConfig & location, ServerConfig & server);
	void _trim(std::string & rawString);
	bool _startsWith(const std::string & str, const std::string & prefix);
	std::vector<std::string> _tokenize(const std::string &rawString);
	size_t _convertDataSize(const std::string &dataSize);
	bool _convertOnOff(const std::string & switchPosition);



};


#endif
