#include "../../includes/config/LocationConfig.hpp"

LocationConfig::LocationConfig() {
	_init();

}

LocationConfig::~LocationConfig() {

}

void LocationConfig::_init() {
	upload_dir = "www/upload";
	index = "index.html";
	autoindex = false;

}


void LocationConfig::printLocConfig() const {
	std::ostringstream ss;
	ss << "\n===================== LOCATION =======================\n";
	ss << "Path: " << path << "\n";
	ss << "Exact match: " << std::boolalpha << exact_match << "\n";
	ss << "Autoindex: " << autoindex << "\n";
	ss << "Root Directory: " << root << "\n";
	ss << "Index Filename: " << index << "\n";
	ss << "CGI extension: " << cgi_extension << "\n";
	ss << "CGI Path: " << cgi_path << "\n";
	ss << "Upload Dir: " << upload_dir << "\n";
	ss << "Return URL: " << return_url << "\n";
	ss << "Methods: " << "\n";
	for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
		ss << "\t" << *it << "\n";
	}
	ss << "===================== END OF LOCATION =======================" << std::endl;
	std::cout << ss.str();
	ss.flush();

}
