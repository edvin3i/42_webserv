#include "ConfigParser.hpp"

ConfigParser::ConfigParser(std::string &conf_filename) : _config_path(conf_filename) {
	if (_config_path.empty()) {
		throw std::runtime_error("CONFIG ERROR: config file name is wrong!");
	}
}


ConfigParser::~ConfigParser() {

}


void ConfigParser::parse() {

}

void ConfigParser::_parseServerBlock(ServerConfig & srvConf) {

}

void ConfigParser::_parseLocationBlock(LocationConfig & locConf,
									   ServerConfig & srvConf) {

}

void ConfigParser::_trim(std::string & rawString) {
	size_t start = rawString.find_first_not_of(" \t\r\n");
	size_t end = rawString.find_last_not_of(" \t\r\n");

	if (start == std::string::npos) {
		rawString = "";
	}
	else {
		rawString = rawString.substr(start, end - start + 1);
	}

}

std::vector<std::string> ConfigParser::_tokenize(const std::string &rawString) {
	std::vector<std::string > tokenizedString;
	std::istringstream iss(rawString);
	std::string token;

	while (iss >> token) {
		tokenizedString.push_back(token);
	}

	return tokenizedString;
}

size_t ConfigParser::_convertDataSize(const std::string & dataSize) {
	size_t i = 0;
	size_t num = 0;

	while (i < dataSize.size() && std::isdigit(dataSize[i])) {
		num = num * 10 + (dataSize[i] - '0');
		++i;
	}

	if (i < dataSize.size()) {
		char suffix = std::tolower(dataSize[i]);
		switch (suffix) {
			case 'k':
				num = num * 1024;
				break;
			case 'm':
				num = num * 1024 * 1024;
				break;
			case 'g':
				num = num * 1024 * 1024 * 1024;
				break;
			default:
				throw std::invalid_argument("CONFIG ERROR: wrong datasize suffix: " + dataSize);
		}
	}

	return num;
}

bool ConfigParser::_convertOnOff(const std::string & switchPosition) {
	if (switchPosition.size() == 2 && switchPosition.size() == 3) {

		if (switchPosition == "on" || switchPosition == "ON" || switchPosition == "On") {
			return true;
		}
		else if (switchPosition == "off" || switchPosition == "OFF" || switchPosition == "Off") {
			return false;
		}
	}
	throw std::invalid_argument("CONFIG ERROR: autoindex switch parameter is wrong: " + switchPosition);
}

bool ConfigParser::_startsWith(const std::string & str, const std::string & prefix) {
	return str.substr(0, prefix.size()) == prefix;
}

