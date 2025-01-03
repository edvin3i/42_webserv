#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include "../logger/Logger.hpp"
#include "../config/ServerConfig.hpp"
#include "../http/RequestParser.hpp"

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

	bool isReadyToWrite();

	void readData();
	void writeData();
	void buildResponse();



private:
	Logger &_logger;
	int _clientSocketFD;
	const ServerConfig &_serverConfig;
	ConnectionState _connectionState;
	std::string _responseMessage; // needs to replace to _responceBuffer
	size_t _writeOffset;
	size_t _currentClientBodySize;
	//	const LocationConfig *_currentLocationConfig;


	// std::vector<char> _readBuffer;
	std::string _readBuffer;
	std::vector<char> _writeBuffer;


};


#endif
