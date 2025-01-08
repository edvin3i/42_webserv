#ifndef STATICRESPONSE_HPP
#define STATICRESPONSE_HPP

#include "Response.hpp"

/*
	StaticResponse class is a response class for static file (.html, .css, etc...) requests
*/

class StaticResponse : public Response {
public:
	StaticResponse(Logger & logger, ServerConfig & serverConfig, Request & request);
	std::string build();

private:
	void _buildHeaders();
	void _buildContent();
};



#endif //STATICRESPONSE_HPP
