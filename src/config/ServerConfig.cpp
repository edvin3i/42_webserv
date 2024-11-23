#include "../../includes/config/ServerConfig.hpp"

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
	error_pages[400] = "html/error_pages/400.html";
	error_pages[403] = "html/error_pages/403.html";
	error_pages[404] = "html/error_pages/404.html";
	error_pages[500] = "html/error_pages/500.html";
	error_pages[501] = "html/error_pages/501.html";


}

void ServerConfig::print_server_config() {
	std::ostringstream ss;
	ss << "===================== SERVER =======================\n";
	ss << "Hostname: " << host << "\n";
	ss << "Port: " << port << "\n";
	ss << "Root Directory: " << root << "\n";
	ss << "Index Filename " << index << "\n";
	ss << "Clent Max Body size: " << client_max_body_size << " bytes" << "\n";
	ss << "Error pages: " << "\n";
	for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
      ss << it->first << " = " << it->second << "; " << "\n";
	}
	ss << "====================================================\n";
	std::cout << ss.str();
	ss.flush();

}
