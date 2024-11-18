#ifndef MASTERSERVER_HPP
#define MASTERSERVER_HPP

#include <string>
#include <vector>
#include <sys/poll.h>
#include "ServerConfig.hpp"
#include "TcpServer.hpp"
#include "ClientConnection.hpp"
#include "Logger.hpp"

class ServerConfig;
class TcpServer;
class ClientConnection;
class Logger;

class MasterServer {
public:
	MasterServer(std::vector<ServerConfig> configs, Logger & logger);
	~MasterServer();

private:
	std::vector<ServerConfig> _configs;
	std::vector<TcpServer> _servers;
	std::vector<pollfd> _fds;
	std::map<int, TcpServer *> _serversMap; // socket_fd->TcpServer
	std::map<int, ClientConnection *> _clientsMap; // socket-fd->ClientConnection

	Logger &_logger;


};


#endif
