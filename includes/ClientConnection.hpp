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
	ClientConnection(Logger & logger, const ServerConfig & config);
	~ClientConnection();


private:
	Logger &_logger;
	const ServerConfig &_serverConfig;
	ConnectionState _connectionState;
//	const LocationConfig *_currentLocationConfig;
	int _clientSocketFD;
	std::vector<char> _readBuffer;
	std::vector<char> _responceBuffer;

};


#endif
