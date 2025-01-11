#include "../../../includes/http/response/ResponseBuilder.hpp"


// Response * ResponseBuilder::buildResponse(Logger &logger,
// 	ServerConfig &serverConfig, Request &request) {
// 	if (_isStaticFileRequest(logger, serverConfig, request)) {
// 		return new StaticResponse(logger, serverConfig, request);
// 	}
// 	if (_isCGIRequest(logger, serverConfig, request)) {
// 		return new CGIResponse(logger, serverConfig, request);
// 	}
// 	ErrorPages::getErrorPage(404); // Just a placeholder.
// 	return NULL;
// }
//
// bool ResponseBuilder::_isStaticFileRequest(Logger &logger,
//                                            ServerConfig &serverConfig, Request &request) {
// 	return false;
// }
//
// bool ResponseBuilder::_isCGIRequest(Logger &logger, ServerConfig &serverConfig,
//                                     Request &request) {
// 	return false;
// }