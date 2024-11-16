#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>

/*
 * Store the "location {}" section parameters
 */

class LocationConfig {

public:
	LocationConfig();
	~LocationConfig();

	std::string path;
	std::vector<std::string > methods;
	std::string root_dir;
	std::string index_file;
	bool autoindex; // "on" or "off" directories listing
	std::string cgi_extension;
	std::string cgi_path;
	std::string upload_dir;
	std::string return_url;

private:
	void _init();

};

#endif
