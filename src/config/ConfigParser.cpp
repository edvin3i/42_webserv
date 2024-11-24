#include <cstdlib>
#include <sstream>
#include "../../includes/config/ConfigParser.hpp"

#define ERR_CONF_FNAME "CONFIG: config file name is wrong!"
#define ERR_CONF_BRACE_OPN "CONFIG: awaiting  '{' after "
#define ERR_CONF_BRACE_CLS "CONFIG: awaiting '}' "
#define ERR_CONF_UNKN_DIRECTIVE "CONFIG: unknown directive "
#define ERR_CONF_UNKN_DIRECTIVE_OR_BLOCK "CONFIG: unknown directive or block on the line number "
#define ERR_CONF_WRNG_SYNTAX "CONFIG: wrong syntax of "
#define ERR_CONF_WRNG_PORT "CONFIG: wrong port number on the line number "
#define ERR_CONF_WRNG_DSIZE "CONFIG: wrong datasize suffix: "
#define ERR_CONF_WRNG_SWSTATE "CONFIG: autoindex state parameter is wrong: "
#define TOKEN_SERVER "server"
#define TOKEN_LOCATION "location"

#define PORT_MIN 1024
#define PORT_MAX 65535


ConfigParser::ConfigParser(Logger &logger, std::string &conf_filename) : _logger(logger), _config_path(conf_filename) {
	if (_config_path.empty()) {
		_handleError(ERR_CONF_FNAME);
	}
}


ConfigParser::~ConfigParser() {
	if (_config_file.is_open()) {
		std::cout << "logfile is open" << std::endl;
		_config_file.close();
	} else {
		std::cout << "logfile is closed" << std::endl;
	}

}


void ConfigParser::parse() {
	_config_file.open(_config_path.c_str());
	if (!_config_file.is_open()) {
		_handleError(ERR_CONF_FNAME + _config_path);
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


		if (_startsWith(_current_line, TOKEN_SERVER)) {
			size_t brace_pos = _current_line.find('{');

			ServerConfig server;

			if (brace_pos == std::string::npos) {
				if (!std::getline(_config_file, _current_line)) {
					std::ostringstream ss;
					ss << ERR_CONF_BRACE_OPN << "'server' on the line number " << _line_number;
					_handleError(ss.str());
				}
			}
			_parseServerBlock(server);
			_servers.push_back(server);
		}
		else {
			std::ostringstream ss;
			ss << ERR_CONF_UNKN_DIRECTIVE_OR_BLOCK << _line_number;
			_handleError(ss.str());
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

		if (_startsWith(_current_line, TOKEN_LOCATION)) {
			size_t brace_pos = _current_line.find('{');

			std::vector<std::string > tokens = _tokenize(_current_line);
			if (tokens.size() < 2) {
				std::ostringstream ss;
				ss << ERR_CONF_WRNG_SYNTAX << "location  on the line number " << _line_number;
				_handleError(ss.str());
			}

			LocationConfig location;
			location.path = tokens[1];

			if (brace_pos == std::string::npos) {
				if (!std::getline(_config_file, _current_line)) {
					std::ostringstream ss;
					ss << ERR_CONF_BRACE_OPN << "'location' on the line number " << _line_number;
					_handleError(ss.str());
				}
				_line_number++;
				_trim(_current_line);
				if (_current_line != "{") {
					std::ostringstream ss;
					ss << ERR_CONF_BRACE_OPN << "'server' on the line number " << _line_number;
					_handleError(ss.str());
				}
			}

			_parseLocationBlock(location);
			server.locations.push_back(location);
		}
		else {
			std::vector<std::string > tokens = _tokenize(_current_line);
			if (tokens.empty())
				continue;
			std::string directive = tokens[0];
			if (directive == "listen") {
				if (tokens.size() != 2) {
					std::ostringstream ss;
					ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number " << _line_number;
					_handleError(ss.str());
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
				if (server.port < PORT_MIN || server.port > PORT_MAX) {
					std::ostringstream ss;
					ss << ERR_CONF_WRNG_PORT << _line_number;
					_handleError(ss.str());
				}
			}
			else if (directive == "server_name") {
				for (size_t i = 1; i < tokens.size(); ++i) {
					server.server_names.push_back(tokens[i]);
				}
			}
			else if (directive == "root") {
				if (tokens.size() != 2) {
					std::ostringstream ss;
					ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number " << _line_number;
					_handleError(ss.str());
				}
				server.root = tokens[1];
			}
			else if (directive == "index") {
				if (tokens.size() != 2) {
					std::ostringstream ss;
					ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number " << _line_number;
					_handleError(ss.str());
				}
				server.index = tokens[1];
			}
			else if (directive == "client_max_body_size") {
				if (tokens.size() != 2) {
					std::ostringstream ss;
					ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number " << _line_number;
					_handleError(ss.str());
				}
				server.client_max_body_size = _convertDataSize(tokens[1]);
			}
			else {
				std::ostringstream ss;
				ss << ERR_CONF_UNKN_DIRECTIVE << directive << " on the line number " << _line_number;
				_handleError(ss.str());
			}
		}
	}
	std::ostringstream ss;
	ss << ERR_CONF_BRACE_OPN << "in the 'server' block but file is ended";
	_handleError(ss.str() );
}

void ConfigParser::_parseLocationBlock(LocationConfig &location) {
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
				std::ostringstream ss;
				ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number " << _line_number << "\n";
				_handleError(ss.str());
			}
			location.upload_dir = tokens[1];
		}
		else if (directive == "index") {
			if (tokens.size() != 2) {
				std::ostringstream ss;
				ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number " << _line_number;
				_handleError(ss.str());
			}
			location.index = tokens[1];
		}
		else if (directive == "root") {
			if (tokens.size() != 2) {
				std::ostringstream ss;
				ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number " << _line_number;
				_handleError(ss.str());
			}
			location.root = tokens[1];
		}
		else if (directive == "cgi_extension") {
			if (tokens.size() != 2) {
				std::ostringstream ss;
				ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number " << _line_number;
				_handleError(ss.str());
			location.cgi_extension = tokens[1];
		}
		else if (directive == "cgi_path") {
			if (tokens.size() != 2) {
				std::ostringstream ss;
				ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number " << _line_number;
				_handleError(ss.str());
			}
			location.cgi_path = tokens[1];
		}
		else if (directive == "return_url") {
			if (tokens.size() != 2) {
				std::ostringstream ss;
				ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number "  << _line_number;
				_handleError(ss.str());
			}
			location.return_url = tokens[1];
		}
		else if (directive == "autoindex") {
			if (tokens.size() != 2) {
				std::ostringstream ss;
				ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number " << _line_number;
				_handleError(ss.str());
			}
			location.autoindex = _convertOnOff(tokens[1]);
		}
		else if (directive == "methods") {
			if (tokens.size() < 2) {
				std::ostringstream ss;
				ss << ERR_CONF_WRNG_SYNTAX << directive << " on the line number " << _line_number;
				_handleError(ss.str());
			}
			for (size_t i = 1; i < tokens.size(); ++i){
				location.methods.push_back(tokens[i]);
			}

		}
		else {
				std::ostringstream ss;
				ss << ERR_CONF_UNKN_DIRECTIVE << directive << " in 'location' on the line number " << _line_number;
				_handleError(ss.str());
		}

		}
	}
	_handleError(ERR_CONF_BRACE_CLS + std::string("in the 'location' block but file is ended =("));
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
				std::string err_msg = ERR_CONF_WRNG_DSIZE + dataSize;
				_logger.writeToLog(ERROR, err_msg);
				throw std::runtime_error(err_msg);
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
	std::string err_msg = ERR_CONF_WRNG_SWSTATE + switchState;
	_logger.writeToLog(ERROR, err_msg);
	throw std::runtime_error(err_msg);
}

bool ConfigParser::_startsWith(const std::string & str, const std::string & prefix) {
	return str.substr(0, prefix.size()) == prefix;
}

void ConfigParser::printConfig() {
	for (size_t i = 0; i < _servers.size(); ++i) {
		std::cout << "Server number " << i << ":" << std::endl;
		_servers[i].print_server_config();
		if (!_servers[i].locations.empty()){
			for (size_t j = 0; j <_servers[i].locations.size(); ++j){
				std::ostringstream ss;
				ss << "===================== LOCATION " << j << " =======================\n";
				ss << "Path: " << _servers[i].locations[j].path.c_str() << "\n";
				ss << "Methods: ";
				for (size_t k = 0; k < _servers[i].locations[j].methods.size(); ++k) {
					ss << " " << _servers[i].locations[j].methods[k];
				}
				ss << "\n";
				ss << "Root Directory: " << _servers[i].locations[j].root << "\n";
				ss << "Index Filename: " << _servers[i].locations[j].index << '\n';
				ss << "Autoindex: " << _servers[i].locations[j].autoindex << "\n";
				ss << "CGI extension: " << _servers[i].locations[j].cgi_extension << "\n";
				ss << "CGI path: " << _servers[i].locations[j].cgi_path << "\n";
				ss << "Upload dir: " << _servers[i].locations[j].upload_dir << "\n";
				ss << "Return URL: " << _servers[i].locations[j].return_url << std::endl;
				std::cout << ss.str();
				ss.flush();
				}
			}
		}
}



std::vector<ServerConfig> ConfigParser::getConfig() {
	return _servers;
}

void ConfigParser::_handleError(const std::string & err_message) {
	std::string err_msg = err_message;
	_logger.writeToLog(ERROR, err_msg);
	throw std::runtime_error(err_msg);
}

