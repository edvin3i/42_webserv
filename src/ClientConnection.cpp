#include "../includes/ClientConnection.hpp"

ClientConnection::ClientConnection(Logger & logger, const ServerConfig & config)
									:	_logger(logger),
										_serverConfig(config),
										//_currentLocationConfig(),
										_clientSocketFD()
									{

}

ClientConnection::~ClientConnection() {

}
