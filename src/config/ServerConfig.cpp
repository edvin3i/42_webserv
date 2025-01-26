#include "../../includes/config/ServerConfig.hpp"

ServerConfig::ServerConfig() {
	_init();

}

ServerConfig::~ServerConfig() {

}

void ServerConfig::_init() {
	host = "127.0.0.1";
	port = 8000;
	root = "www/website";
	index = "index.html";
	client_max_body_size = 512 * 1024;
}

void ServerConfig::print_server_config() const{
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
	std::cout << ss.str();
	ss.str("");

	// Print locations configs. Yes, I know that it seems like a spaghetti :-/
	if (!locations.empty()) {
		ss << "Locations: " << "\n";
		std::cout << ss.str();
		ss.str("");

		for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
			std::cout << "\nLocation path " << it->first << ": " << std::endl;
			it->second.printLocConfig();
			// it->printLocConfig();
		}
	}

	ss << "\n===================== END OF SERVER =======================\n";
	std::cout << ss.str();
	ss.str("");

}
