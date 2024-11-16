#include "../includes/ServerConfig.hpp"

ServerConfig::ServerConfig() {
	_init();

}

ServerConfig::~ServerConfig() {

}

void ServerConfig::_init() {
	host = "127.0.0.1";
	port = 8080;
	root = "www";
	index = "index.html";
	client_max_body_size = 512 * 1024;
	error_pages[404] = "html/error_pages/404.html";


}

void ServerConfig::print_server_config() {
	std::cout << "===================== SERVER =======================\n" << std::endl;
	std::cout << "Hostname: " << host << std::endl;
	std::cout << "Port: " << port << std::endl;
	std::cout << "Root Directory: " << root << std::endl;
	std::cout << "Index Filename " << index << std::endl;
	std::cout << "Clent Max Body size: " << client_max_body_size << " bytes" << std::endl;
	std::cout << "Error pages:" << std::endl;
	for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
      std::cout << it->first << " = " << it->second << "; " << std::endl;

	}
	std::cout << "====================================================\n" << std::endl;

}
