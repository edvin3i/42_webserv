#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include <string>
#include "Response.hpp"
#include "../Request.hpp"
#include "../../logger/Logger.hpp"
#include "../../config/ServerConfig.hpp"



class ResponseBuilder {
  public:
    static Response *buildResponse(Logger *logger, ServerConfig *serverConfig, Request *request);

};



#endif //RESPONCEBUILDER_HPP
