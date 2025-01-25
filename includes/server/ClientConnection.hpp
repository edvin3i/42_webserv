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
#include "../http/Response.hpp"
#include "../Utils.hpp"
#include "../Env.hpp"


const int BUFFER_SIZE = 8192;


class Logger;
class Request;

enum ConnectionState {
	READING,
	WRITING,
	CLOSING
};

enum ReadingState
{
	READ_REQUEST_LINE_HEADERS,
	READ_CONTENT,
	READ_CONTENT_CHUNKED,
};

class ClientConnection {
public:
	ClientConnection(Logger & logger, int socketFD, const ServerConfig & config, Env & env);
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
	// void select_server_config(const std::vector<ServerConfig>&);
	bool keep_alive() const;
	void initRequest();

private:
	void _setRequestLineHeaders(const std::string& str);
	void _checkContentLength(size_t&, bool&);
	void _checkChunked(bool&);
	void _readRequestLineHeaders(std::string& readBuffer, std::string& content_begin);
	void _readContent(std::string& readBuffer, size_t content_length);
	void _readChunkedContent(std::string& readBuffer);

private:
	Logger &_logger;
	int _clientSocketFD;
	const ServerConfig *_currentServerConfig;
	ConnectionState _connectionState;
	std::string _responseMessage;
	size_t _writeOffset;
	// size_t _currentClientBodySize;
	LocationConfig *_currentLocationConfig;
	Request *_request;
	Response *_response;
	Env &_env;
	bool _keep_alive;


	std::string _readBuffer;
	std::vector<char> _writeBuffer;


};


#endif
