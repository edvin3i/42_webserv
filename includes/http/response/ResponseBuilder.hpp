#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include <string>
#include "Response.hpp"
#include "CGIResponse.hpp"
#include "StaticResponse.hpp"
#include "../Request.hpp"
#include "../../logger/Logger.hpp"
#include "../../config/ServerConfig.hpp"

/*
  ResponseBuilder class is a factory class for creating response objects
  It chooses the specific response class based on the request
*/

class ResponseBuilder {
  public:
    static Response *buildResponse(Logger & logger, ServerConfig & serverConfig, Request & request);

  private:
    static bool _isCGIRequest(Logger & logger, ServerConfig & serverConfig, Request & request);
    static bool _isStaticFileRequest(Logger & logger, ServerConfig & serverConfig, Request & request);

};



#endif //RESPONCEBUILDER_HPP
