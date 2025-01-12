#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>

#include "ErrorPages.hpp"
#include "../Message.hpp"
#include "../StatusLine.hpp"
#include "../Request.hpp"
#include "../../logger/Logger.hpp"
#include "../../../includes/config/ServerConfig.hpp"

/*
	Response class is a base class for all response classes (CGIResponse, StaticResponse, etc.)
	It contains all the common methods and fields for all response classes
	Specific response class chooses at the ResponseBuilder class (factory)
*/


class Response : public Message<StatusLine>
{
public:
	Response(Logger & logger, const ServerConfig & srv_conf, const Request & request);
	Response(const Request&);
	virtual ~Response();

	virtual std::string build() = 0;
	const std::string& str() const;

protected:
	Logger &_logger;
	const Request &_request;
	const ServerConfig &_conf;


	Response();
	Response(const Response & other);
	Response &operator=(const Response & other);
	void _buildStatusLine(int code);
	void _addHeader(std::string key, std::string val);
//	void _buildContent();
private:
	static void _init_status_code_message();
	static std::map<int, std::string> _status_code_message;
	std::string _str_content;
};


#endif //RESPONSE_HPP
