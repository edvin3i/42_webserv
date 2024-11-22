#include "../../includes/server/MasterServer.hpp"

const int TIMEOUT = 32;

MasterServer::MasterServer(Logger & logger, const std::vector<ServerConfig> & configs)
						: _logger(logger),
						  _configs(configs) {

	std::ostringstream oss;
	oss << "MasterServer constructor called!\n";
	oss << "Size of configs: " << _configs.size() << "\n";
	_logger.writeToLog(DEBUG, oss.str());

	_fds.reserve(_configs.size());
	_servers.reserve(_configs.size());

	// Creating new TcpServer instances in the 'for' loop
	for (size_t i = 0; i < _configs.size(); ++i) {
		_servers.push_back(
				new TcpServer(
						logger,
						_configs[i])
						);

		_servers.back()->startServer();

		std::ostringstream ss;
		ss << "Created server number: " << i;
		_logger.writeToLog(DEBUG, ss.str());

		// Creating new pollfd element and setup it
		pollfd server_fd;
		server_fd.fd = _servers.back()->getSrvSocket();
		server_fd.events = POLLIN;
		server_fd.revents = 0;

		// Adding the new element to list of pollfds
		_fds.push_back(server_fd);

		// Adding new server instance to the map of servers
		_serversMap[server_fd.fd] = _servers.back();

		// Set the server to listen mode
		_servers.back()->startListen();
	}
}

MasterServer::~MasterServer() {
	for (size_t i = 0; i < _servers.size(); ++i) {
		delete _servers[i];
	}
	_servers.clear();
}

void MasterServer::run() {
	while(true) {
		int polling = poll(_fds.data(), _fds.size(), TIMEOUT);
		if (polling < 0) {
			_logger.writeToLog(ERROR, "ERROR: poll() return -1!");
			break;
		}

		for (size_t i = 0; i < _fds.size(); ++i) {
			if (_serversMap.find(_fds[i].fd) != _serversMap.end()) {
				TcpServer *server = _serversMap[_fds[i].fd];
				short revents = _fds[i].revents;

				if (revents & POLLIN) {
					int new_socket = server->acceptConnection();
					if (new_socket >= 0) {
						fcntl(new_socket, F_SETFL, O_NONBLOCK);

						pollfd client_fd;
						client_fd.fd = new_socket;
						client_fd.events = POLLIN;
						client_fd.revents = 0;

						_fds.push_back(client_fd);
						_clientsMap[new_socket] = new ClientConnection(
																_logger,
																client_fd.fd,
																_serversMap[_fds[i].fd]->getConfig()
																);


					}
				}
			}
			else if (_clientsMap.find(_fds[i].fd) != _clientsMap.end()) {
				ClientConnection *client = _clientsMap[_fds[i].fd];
				short revents = _fds[i].revents;

				switch (client->getState()) {
					case READING:
						if (revents & POLLIN) {
							std::cout << "Case READING" << std::endl;
							client->readData();
							_fds[i].events = POLLOUT;
							client->setState(WRITING);
						}
						if (revents & POLLERR) {
							client->setState(CLOSING);
						}
						break;
					case WRITING:
						if (revents & POLLOUT) {
							std::cout << "Case WRITING" << std::endl;
							client->buildResponse();
							client->sendResponse();
							client->setState(CLOSING);
						}
						if (revents & (POLLERR | POLLHUP)) {
							client->setState(CLOSING);
						}
						break;
					case CLOSING:
						std::cout << "Case CLOSING" << std::endl;
//						client->closeConnection();
						close(_fds[i].fd);
						delete client;
						_clientsMap.erase(_fds[i].fd);
						_fds.erase(_fds.begin() + i);
						--i;
						break;
				}
			}
		}
	}
}