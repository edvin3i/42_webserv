#include "../includes/MasterServer.hpp"

MasterServer::MasterServer(std::vector<ServerConfig> configs, Logger & logger): _configs(configs), _logger(logger) {

	std::ostringstream oss;
	oss << "MasterServer constructor called!\n";
	oss << "Size of configs: " << _configs.size() << "\n";
	_logger.writeToLog(oss.str());

	for (size_t i = 0; i < _configs.size(); ++i) {

		_servers.push_back(
				TcpServer(_configs[i].host,
						 _configs[i].port,
						 logger)
						 );

		std::ostringstream ss;
		ss << "Created server number: " << _servers.size();
		_logger.writeToLog(ss.str());


	}
}

MasterServer::~MasterServer() {

}
