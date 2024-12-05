#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include "LocationConfig.hpp"

class LocationConfig;

class ServerConfig {

public:
	ServerConfig();
	~ServerConfig();

	void print_server_config() const;

	std::string host;
	int port; // need to check range 1024-65535
	std::vector<std::string > server_names;
	std::string root;
	std::string index;
	std::map<int, std::string > error_pages;
	size_t client_max_body_size;
	std::vector<LocationConfig > locations;

private:
	void _init();

};


#endif
