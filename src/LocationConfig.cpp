#include "../includes/LocationConfig.hpp"

LocationConfig::LocationConfig() {
	_init();

}

LocationConfig::~LocationConfig() {

}

void LocationConfig::_init() {
	upload_dir = "www/upload";
	index = "index.html";
	autoindex = false;
	methods.push_back("GET");
	methods.push_back("PUT");
	methods.push_back("POST");
	methods.push_back("DELETE");

}
