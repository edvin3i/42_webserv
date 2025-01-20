#ifndef CLIENTCONNECTION_HPP
#define CLIENTCONNECTION_HPP

#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <algorithm>
#include "../logger/Logger.hpp"
#include "../config/ServerConfig.hpp"
#include "../http/response/Response.hpp"
#include "../http/Utils.hpp"


const int BUFFER_SIZE = 8192;


class Logger;
class Request;

enum ConnectionState {
	READING,
	WRITING,
	CLOSING
};

class ClientConnection {
public:
	ClientConnection(Logger & logger, int socketFD, const ServerConfig & config, char **env);
	~ClientConnection();

	ConnectionState getState() const;
	void setState(ConnectionState state);

	bool isReadyToWrite();

	void readData();
	void writeData();
	void buildResponse();
	const Request* getRequest() const;

	void setRequest();
	void select_location();


	char **env;
private:
	Logger &_logger;
	int _clientSocketFD;
	const ServerConfig *_currentServerConfig;
	ConnectionState _connectionState;
	std::string _responseMessage; // needs to replace to _responceBuffer
	size_t _writeOffset;
	size_t _currentClientBodySize;
	LocationConfig *_currentLocationConfig;
	Request *_request;
	Response *_response;


	std::string _readBuffer;
	std::vector<char> _writeBuffer;


};


#endif
