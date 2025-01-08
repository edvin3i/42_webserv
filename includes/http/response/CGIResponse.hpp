#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

#include "Response.hpp"

/*
	CGIResponse class is a response class for CGI requests
*/

class CGIResponse : public Response {
public:
	CGIResponse(Logger & logger, const ServerConfig & serverConfig, const Request & request);
	std::string build();

private:
	void _buildHeaders();
	void _executeCGI();
};



#endif //CGIRESPONSE_HPP
