#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <map>
#include <vector>
#include <string>
#include "LocationConfig.hpp"

class LocationConfig;

class ServerConfig {

public:
	ServerConfig();
	~ServerConfig();

	std::string host;
	int port; // need to check range 1-65535
	std::vector<std::string > server_names;
	std::string root_dir;
	std::map<int, std::string > error_pages;
	size_t client_max_body_size;
	std::vector<LocationConfig > locations;

private:
	void _init();

};


#endif
