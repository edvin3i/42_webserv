#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "Logger.hpp"
#include "ServerConfig.hpp"

class Logger;

enum ConnectionState {
	READING,
	WRITING,
	CLOSED
};

class ClientConnection {
public:
	ClientConnection(const ServerConfig & config, Logger & logger);
	~ClientConnection();


private:
	int _clientSocketFD;
	std::vector<char> _readBuffer;
	std::vector<char> _responceBuffer;
	ConnectionState _connectionState;
	Logger &_logger;

	const ServerConfig &_serverConfig;
	const LocationConfig *_currentLocationConfig;

};


#endif
