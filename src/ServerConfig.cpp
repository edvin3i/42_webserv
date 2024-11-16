#include "../includes/ServerConfig.hpp"

ServerConfig::ServerConfig() {
	_init();

}

ServerConfig::~ServerConfig() {

}

void ServerConfig::_init() {
	host = "127.0.0.1";
	port = 8080;
	root_dir = "www";
	index = "index.html";
	error_pages[404] = "html/error_pages/404.html";


}
