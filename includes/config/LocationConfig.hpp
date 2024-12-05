#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

/*
 * Store the "location {}" section parameters
 */

class LocationConfig {

public:
	LocationConfig();
	~LocationConfig();

	void printLocConfig() const;

	std::string path;
	std::vector<std::string> methods;
	std::string root;
	std::string index;
	bool autoindex; // "on" or "off" directories listing
	std::string cgi_extension;
	std::string cgi_path;
	std::string upload_dir;
	std::string return_url;

private:
	void _init();

};

#endif
