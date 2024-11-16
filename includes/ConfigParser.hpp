#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cctype>
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
	void _parseServerBlock(ServerConfig & srvConf);
	void _parseLocationBlock(LocationConfig & locConf, ServerConfig & srvConf);
	void _trim(std::string & rawString);
	bool _startsWith(const std::string & str, const std::string & prefix);
	std::vector<std::string> _tokenize(const std::string &rawString);
	size_t _convertDataSize(const std::string &dataSize);
	bool _convertOnOff(const std::string & switchPosition);



};


#endif
