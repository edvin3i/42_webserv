#include "../../includes/server/MasterServer.hpp"


const int TIMEOUT = 32;

MasterServer::MasterServer(Logger & logger, const std::vector<ServerConfig> & configs, char **env)
						: _logger(logger), _configs(configs), _servers(), _env(env), _fds(), _serversMap(), _clientsMap() {

	try
	{
		std::ostringstream oss;
		oss << "MasterServer constructor called!\t";
		oss << "Size of configs: " << _configs.size() << "\n";
		MimeType::init_mime_type();
		_logger.writeToLog(DEBUG, oss.str());

		_fds.reserve(_configs.size());
		_servers.reserve(_configs.size());

		// Creating new TcpServer instances in the 'for' loop
		for (size_t i = 0; i < _configs.size(); ++i) {
			TcpServer *new_server = new TcpServer(logger, _configs[i]);
			_servers.push_back(new_server);

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

		std::ostringstream os;
		os << "Size of _fds = " << _fds.size();
		_logger.writeToLog(DEBUG, os.str());
	}
	catch (const std::exception& e)
	{
		_free();
		throw (std::runtime_error(e.what()));
	}
}

MasterServer::~MasterServer()
{
	_free();
}

void MasterServer::_free()
{
	for (size_t i = 0; i < _servers.size(); ++i) {
		delete _servers[i];
	}
	_servers.clear();

	for (std::map<int, ClientConnection* >::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it) {
		delete it->second;
	}
	_clientsMap.clear();
}

void MasterServer::run() {
	while(!g_sig) {
		int polling = poll(_fds.data(), _fds.size(), TIMEOUT);
		if (polling < 0) {
			if (errno == EINTR) {continue;} // added checking of errno for "interrupted by signal"
			_logger.writeToLog(ERROR, "poll() return -1!");
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
						client_fd.events = POLLIN | POLLOUT; // add POLLOUT check
						client_fd.revents = 0;

						_fds.push_back(client_fd);
						_clientsMap[new_socket] = new ClientConnection(
																_logger,
																client_fd.fd,
																_serversMap[_fds[i].fd]->getConfig(),
																_env);

					}
				}
			}
			else if (_clientsMap.find(_fds[i].fd) != _clientsMap.end()) {
				ClientConnection *client = _clientsMap[_fds[i].fd];
				short revents = _fds[i].revents;

				switch (client->getState()) {
					case READING:
						if (revents & POLLIN) {
							client->readData();
							_fds[i].events = POLLOUT;
							client->setState(WRITING);
						}
						if (revents & (POLLERR | POLLHUP)) {
							client->setState(CLOSING);
							continue;
						}
						break;
					case WRITING:
						if (revents & POLLOUT) {

							client->setRequest();
							if (!client->getRequest()->error())
							{
								// client->select_server_config(_configs);
								client->select_location();
							}
							client->buildResponse();

							// continue sending data until all data is sent
							while (client->isReadyToWrite()) {
								client->writeData();
							}

							if (client->getState() != WRITING || !client->keep_alive()) {
								client->setState(CLOSING);
							}
							_fds[i].events = POLLIN; // set POLLIN
						}
						if (revents & (POLLERR | POLLHUP)) {
							client->setState(CLOSING);
							continue;
						}
						break;
					case CLOSING:
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
	stop();
}


void MasterServer::stop() {
	for (size_t i = 0; i < _fds.size(); ++i) {
		if (close(_fds[i].fd) == 0) {
			std::ostringstream oss;
			oss << "Closed connection: i = " << i << ", FD = " << _fds[i].fd;
			_logger.writeToLog(DEBUG, oss.str());
		}
		else {
			std::ostringstream oss;
			oss << "Closing connection problem!" << " i = " << i << ", FD = " << _fds[i].fd;
			_logger.writeToLog(DEBUG, oss.str());
		}
	}
}
