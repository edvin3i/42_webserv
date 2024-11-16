#include <cstdlib>
#include "ConfigParser.hpp"

ConfigParser::ConfigParser(std::string &conf_filename) : _config_path(conf_filename) {
	if (_config_path.empty()) {
		throw std::runtime_error("CONFIG ERROR: config file name is wrong!");
	}
}


ConfigParser::~ConfigParser() {

}


void ConfigParser::parse() {
	_config_file.open(_config_path.c_str());
	if (!_config_file.is_open()) {
		throw std::runtime_error("ERROR: can not open the configuraton file" + _config_path);
	}

	_line_number = 0;
	while (std::getline(_config_file, _current_line)) {
		_line_number++;

		// Del comments
		size_t comment_pos = _current_line.find('#');
		if (comment_pos != std::string::npos) {
			_current_line = _current_line.substr(0, comment_pos);
		}

		// trim whitespaces
		_trim(_current_line);
		if (_current_line.empty())
			continue;


		if (_startsWith(_current_line, "server")) {
			size_t brace_pos = _current_line.find('{');

			ServerConfig server;

			if (brace_pos == std::string::npos) {
				if (!std::getline(_config_file, _current_line)) {
					throw std::runtime_error("CONFIG ERROR: awaiting  '{' after 'server' on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
				}
			}
			_parseServerBlock(server);
			_servers.push_back(server);
		}
		else {
			throw std::runtime_error("CONFIG ERROR: unknown directive or block on the line number " + \
				static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
		}
	}
	_config_file.close();
}

void ConfigParser::_parseServerBlock(ServerConfig & server) {
	while(std::getline(_config_file, _current_line)) {
		_line_number++;

		size_t comment_pos = _current_line.find('#');
		if (comment_pos != std::string::npos) {
			_current_line = _current_line.substr(0, comment_pos);
		}
		_trim(_current_line);
		if (_current_line.empty())
			continue;
		if (_current_line == "}")
			return;

		if (_startsWith(_current_line, "location")) {
			size_t brace_pos = _current_line.find('{');

			std::vector<std::string > tokens = _tokenize(_current_line);
			if (tokens.size() < 2) {
				throw std::runtime_error("CONFIG ERROR: wrong syntax of 'location' on the line number " + \
				static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
			}
			LocationConfig location;
			location.path = tokens[1];

			if (brace_pos == std::string::npos) {
				if (!std::getline(_config_file, _current_line)) {
					throw std::runtime_error("CONFIG ERROR: awaiting  '{' after 'location' on the line number " + \
					static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
				}
				_line_number++;
				_trim(_current_line);
				if (_current_line != "{") {
					throw std::runtime_error("CONFIG ERROR: awaiting  '{' after 'server' on the line number " + \
				static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
				}
			}
			_parseLocationBlock(location, server);
			server.locations.push_back(location);
		}
		else {
			std::vector<std::string > tokens = _tokenize(_current_line);
			if (tokens.empty())
				continue;
			std::string directive = tokens[0];
			if (directive == "listen") {
				if (tokens.size() != 2) {
					throw std::runtime_error("CONFIG ERROR: wrong syntax in 'listen' on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
				}
				std::string listen_value = tokens[1];
				size_t colon_pos = listen_value.find(':');
				if (colon_pos != std::string::npos) {
					server.host = listen_value.substr(0, colon_pos);
					server.port = atoi(listen_value.substr(colon_pos + 1).c_str());
				}
				else {
					server.port = atoi(listen_value.c_str());
				}
				if (server.port < 1 || server.port > 65535) {
					throw std::runtime_error("CONFIG ERROR: wrong port number on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
				}
			}
			else if (directive == "server_name") {
				for (size_t i = 1; i < tokens.size(); ++i) {
					server.server_names.push_back(tokens[i]);
				}
			}
			else if (directive == "root") {
				if (tokens.size() != 2) {
					throw std::runtime_error("CONFIG ERROR: wrong root syntax on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
				}
				server.root = tokens[1];
			}
			else if (directive == "index") {
				if (tokens.size() != 2) {
					throw std::runtime_error("CONFIG ERROR: wrong index syntax on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
				}
				server.index = tokens[1];
			}
			else if (directive == "client_max_body_size") {
				if (tokens.size() != 2) {
					throw std::runtime_error("CONFIG ERROR: wrong client max body size syntax on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
				}
				server.client_max_body_size = _convertDataSize(tokens[1]);
			}
			else {
				std::cout << directive << std::endl;
				throw std::runtime_error("CONFIG ERROR: unknown directive on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
			}
		}
	}
	throw std::runtime_error("CONFIG ERROR: awaiting '}' in the 'server' block but file is ended");
}

void ConfigParser::_parseLocationBlock(LocationConfig & location, ServerConfig & server) {
	while(std::getline(_config_file, _current_line)) {
		++_line_number;

		size_t comment_pos = _current_line.find('#');
		if (comment_pos != std::string::npos) {
			_current_line = _current_line.substr(0, comment_pos);
		}

		_trim(_current_line);
		if (_current_line.empty())
			continue;
		if (_current_line == "}")
			return;
		std::vector<std::string> tokens = _tokenize(_current_line);
		if (tokens.empty())
			continue;

		std::string directive = tokens[0];
		if (directive == "upload_dir") {
			if (tokens.size() != 2) {
				throw std::runtime_error("CONFIG ERROR: wrong syntax in 'upload_dir' on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
			}
			location.upload_dir = tokens[1];
		}
		else if (directive == "index") {
			if (tokens.size() != 2) {
				throw std::runtime_error("CONFIG ERROR: wrong syntax in 'index' on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
			}
			location.index = tokens[1];
		}
		else if (directive == "root") {
			if (tokens.size() != 2) {
				throw std::runtime_error("CONFIG ERROR: wrong syntax in 'root' on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
			}
			location.root = tokens[1];
		}
		else if (directive == "cgi_extension") {
			if (tokens.size() != 2) {
				throw std::runtime_error("CONFIG ERROR: wrong syntax in 'cgi_extension' on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
			}
			location.cgi_extension = tokens[1];
		}
		else if (directive == "cgi_path") {
			if (tokens.size() != 2) {
				throw std::runtime_error("CONFIG ERROR: wrong syntax in 'cgi_path' on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
			}
			location.cgi_path = tokens[1];
		}
		else if (directive == "return_url") {
			if (tokens.size() != 2) {
				throw std::runtime_error("CONFIG ERROR: wrong syntax in 'return_url' on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
			}
			location.return_url = tokens[1];
		}
		else if (directive == "autoindex") {
			if (tokens.size() != 2) {
				throw std::runtime_error("CONFIG ERROR: wrong syntax in 'autoindex' on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
			}
			location.autoindex = _convertOnOff(tokens[1]);
		}
		else if (directive == "methods") {
			if (tokens.size() < 2) {
				throw std::runtime_error("CONFIG ERROR: wrong syntax in 'methods' on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
			}
			for (size_t i = 1; i < tokens.size(); ++i){
				location.methods.push_back(tokens[i]);
			}

		}
		else {
			std::cout << directive << std::endl;
			throw std::runtime_error("CONFIG ERROR: unknown directive in 'location' on the line number " + \
						static_cast<std::ostringstream *> (&(std::ostringstream() << _line_number))->str());
		}




	}
	throw std::runtime_error("CONFIG ERROR: awaiting '}' in the 'location' block but file is ended");
}



/*
 * Utilites
 */

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

bool ConfigParser::_convertOnOff(const std::string & switchState) {
	if (switchState.size() == 2 || switchState.size() == 3) {

		if (switchState == "on" || switchState == "ON" || switchState == "On") {
			return true;
		}
		else if (switchState == "off" || switchState == "OFF" || switchState == "Off") {
			return false;
		}
	}
	throw std::invalid_argument("CONFIG ERROR: autoindex state parameter is wrong: " + switchState);
}

bool ConfigParser::_startsWith(const std::string & str, const std::string & prefix) {
	return str.substr(0, prefix.size()) == prefix;
}

void ConfigParser::printConfig() {
	for (size_t i = 0; i < _servers.size(); ++i) {
		std::cout << "Server number " << i << ":" << std::endl;
		_servers[i].print_server_config();
	}

}

std::vector<ServerConfig> ConfigParser::getConfig() {
	return _servers;
}

