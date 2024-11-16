#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <map>
#include <vector>
#include <string>


class ServerConfig {

public:
	ServerConfig();
	~ServerConfig();

	std::vector<std::map<std::string, std::string> > getServerParams(int n);
	int setServerParams(int n, std::map<std::string, std::string>);

private:
	std::vector<std::map<std::string, std::string> > _server_config;

};


#endif
