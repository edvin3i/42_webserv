#include "../includes/MasterServer.hpp"

MasterServer::MasterServer(Logger & logger, const std::vector<ServerConfig> & configs)
						: _logger(logger),
						  _configs(configs) {

	std::ostringstream oss;
	oss << "MasterServer constructor called!\n";
	oss << "Size of configs: " << _configs.size() << "\n";
	_logger.writeToLog(oss.str());

	_servers.reserve(_configs.size());

	// Creating new TcpServer instances in the for loop
	for (size_t i = 0; i < _configs.size(); ++i) {
		_servers.push_back(
				new TcpServer(
						logger, _configs[i].host,
						_configs[i].port)
						 );

		_servers.back()->start();

		std::ostringstream ss;
		ss << "Created server number: " << i;
		_logger.writeToLog(ss.str());

		// Creating new pollfd element and setup it
		pollfd server_fd;
		server_fd.fd = _servers.back()->getSrvSocket();
		server_fd.events = POLLIN;
		server_fd.revents = 0;

		// Adding the new element to list of pollfds
		_fds.push_back(server_fd);



		//_servers.back()->startListen(); // DOES NOT WORK. Need to use poll()
	}
}

MasterServer::~MasterServer() {
	for (size_t i = 0; i < _servers.size(); ++i) {
		delete _servers[i];
	}
}
