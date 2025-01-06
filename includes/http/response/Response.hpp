#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>

#include "../Message.hpp"
#include "../StatusLine.hpp"
#include "../Request.hpp"
#include "../../logger/Logger.hpp"
#include "../includes/config/ServerConfig.hpp"

class Response : public Message<StatusLine>
{
public:
	Response(Logger & logger, const ServerConfig & srv_conf, const Request & request);
	~Response();

	std::string build();

private:
	Logger &_logger;
	const Request &_request;
	const ServerConfig &_conf;

	Response();
	Response(const Response & other);
	Response &operator=(const Response & other);

	void _buildStatusLine();
	void _buildHeader();
	void _buildContent();
};



#endif //RESPONSE_HPP
