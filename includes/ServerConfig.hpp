#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <map>
#include <vector>
#include <string>

using ServerParams = std::map<std::string, std::string>;

class ServerConfig {

public:
	ServerConfig();
	~ServerConfig();

	std::vector<ServerParams> getServerParams(int n);
	int setServerParams(int n, ServerParams);

private:
	std::vector<ServerParams> _server_config;

};


#endif
