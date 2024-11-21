#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <iostream>
#include "Logger.hpp"
#include "ServerConfig.hpp"

const int BUFFER_SIZE = 8192;


class Logger;

enum ConnectionState {
	READING,
	WRITING,
	CLOSING
};

class ClientConnection {
public:
	ClientConnection(Logger & logger, const ServerConfig & config);
	~ClientConnection();
	void readData();
	void buildResponse();
	void sendResponse();
	void setState(ConnectionState state);



private:
	Logger &_logger;
	const ServerConfig &_serverConfig;
	ConnectionState _connectionState;
//	const LocationConfig *_currentLocationConfig;
	int _clientSocketFD;
	int _newServerSocket;
	std::string _responseMessage;


	std::vector<char> _readBuffer;
	std::vector<char> _responseBuffer;

};


#endif
