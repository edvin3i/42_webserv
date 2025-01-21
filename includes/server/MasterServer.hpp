#ifndef MASTERSERVER_HPP
#define MASTERSERVER_HPP

#include <string>
#include <vector>
#include <sys/poll.h>
#include "../signals/signals.hpp"
#include "../config/ServerConfig.hpp"
#include "TcpServer.hpp"
#include "ClientConnection.hpp"
#include "../logger/Logger.hpp"
#include "../http/MimeType.hpp"
#include "../Env.hpp"

class ServerConfig;
class TcpServer;
class ClientConnection;
class Logger;

class MasterServer {
public:
	MasterServer(Logger & logger, const std::vector<ServerConfig> & configs, char **env);
	~MasterServer();
	void run();
	void stop();

private:

	Logger &_logger;

	std::vector<ServerConfig> _configs;
	std::vector<TcpServer *> _servers;
	Env _env;

	std::vector<pollfd> _fds;
	std::map<int, TcpServer *> _serversMap; // socket_fd->TcpServer
	std::map<int, ClientConnection *> _clientsMap; // socket-fd->ClientConnection
};


#endif
