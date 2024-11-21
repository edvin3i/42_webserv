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
	ClientConnection(Logger & logger, int socketFD, const ServerConfig & config);
	~ClientConnection();

	ConnectionState getState() const;
	void setState(ConnectionState state);

	void readData();
	void buildResponse();
	void sendResponse();
	void closeConnection();



private:
	Logger &_logger;
	int _clientSocketFD;
	const ServerConfig &_serverConfig;
	ConnectionState _connectionState;
	std::string _responseMessage; // needs to replace to _responceBuffer
	//	int _newServerSocket;
	//	const LocationConfig *_currentLocationConfig;


	std::vector<char> _readBuffer;
	std::vector<char> _responseBuffer;

};


#endif
