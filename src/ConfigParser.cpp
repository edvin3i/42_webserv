#include <cstdlib>
#include <sstream>
#include "ConfigParser.hpp"

#define ERR_CONF_FNAME "CONFIG ERROR: config file name is wrong!"
#define ERR_CONF_BRACE_OPN "CONFIG ERROR: awaiting  '{' after "
#define ERR_CONF_BRACE_CLS "CONFIG ERROR: awaiting '}' "
#define ERR_CONF_UNKN_DIRECTIVE "CONFIG ERROR: unknown directive "
#define ERR_CONF_UNKN_DIRECTIVE_OR_BLOCK "CONFIG ERROR: unknown directive or block on the line number "
#define ERR_CONF_WRNG_SYNTAX "CONFIG ERROR: wrong syntax of "
#define ERR_CONF_WRNG_PORT "CONFIG ERROR: wrong port number on the line number "
#define ERR_CONF_WRNG_DSIZE "CONFIG ERROR: wrong datasize suffix: "
#define ERR_CONF_WRNG_SWSTATE "CONFIG ERROR: autoindex state parameter is wrong: "


#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()


ConfigParser::ConfigParser(std::string &conf_filename, Logger & logger) : _config_path(conf_filename), _logger(logger) {
	if (_config_path.empty()) {
		_logger.writeToLog(ERR_CONF_FNAME);
		throw std::runtime_error(ERR_CONF_FNAME);
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
		_logger.writeToLog(ERR_CONF_FNAME);
		throw std::runtime_error(ERR_CONF_FNAME + _config_path);
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
					std::string err_msg = ERR_CONF_BRACE_OPN + std::string("'server' on the line number ") + SSTR(_line_number);
					_logger.writeToLog(err_msg);
					throw std::runtime_error(err_msg);
				}
			}
			_parseServerBlock(server);
			_servers.push_back(server);
		}
		else {
			std::string err_msg = ERR_CONF_UNKN_DIRECTIVE_OR_BLOCK + SSTR(_line_number);
			_logger.writeToLog(err_msg);
			_logger.closeLogFile();
			throw std::runtime_error(err_msg);
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
				std::string err_msg = ERR_CONF_WRNG_SYNTAX + std::string("location  on the line number ") + SSTR(_line_number);
				_logger.writeToLog(err_msg);
				throw std::runtime_error(err_msg);
			}
			LocationConfig location;
			location.path = tokens[1];

			if (brace_pos == std::string::npos) {
				if (!std::getline(_config_file, _current_line)) {
					std::string err_msg = ERR_CONF_BRACE_OPN + std::string("'location' on the line number ") + SSTR(_line_number);
					_logger.writeToLog(err_msg);
					throw std::runtime_error(err_msg);;
				}
				_line_number++;
				_trim(_current_line);
				if (_current_line != "{") {
					std::string err_msg = ERR_CONF_BRACE_OPN + std::string("'server' on the line number ") + SSTR(_line_number);
					_logger.writeToLog(err_msg);
					throw std::runtime_error(err_msg);
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
					std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
					_logger.writeToLog(err_msg);
					throw std::runtime_error(err_msg);
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
				if (server.port < 1024 || server.port > 65535) {
					std::string err_msg = ERR_CONF_WRNG_PORT + SSTR(_line_number);
					_logger.writeToLog(err_msg);
					throw std::runtime_error(err_msg);
				}
			}
			else if (directive == "server_name") {
				for (size_t i = 1; i < tokens.size(); ++i) {
					server.server_names.push_back(tokens[i]);
				}
			}
			else if (directive == "root") {
				if (tokens.size() != 2) {
					std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
					_logger.writeToLog(err_msg);
					throw std::runtime_error(err_msg);
				}
				server.root = tokens[1];
			}
			else if (directive == "index") {
				if (tokens.size() != 2) {
					std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
					_logger.writeToLog(err_msg);
					throw std::runtime_error(err_msg);
				}
				server.index = tokens[1];
			}
			else if (directive == "client_max_body_size") {
				if (tokens.size() != 2) {
					std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
					_logger.writeToLog(err_msg);
					throw std::runtime_error(err_msg);
				}
				server.client_max_body_size = _convertDataSize(tokens[1]);
			}
			else {
				std::string err_msg = ERR_CONF_UNKN_DIRECTIVE + directive + " on the line number " + SSTR(_line_number);
				_logger.writeToLog(err_msg);
				throw std::runtime_error(err_msg);
			}
		}
	}
	std::string err_msg = ERR_CONF_BRACE_OPN + std::string("in the 'server' block but file is ended");
	_logger.writeToLog(err_msg);
	throw std::runtime_error(err_msg);
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
				std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
				_logger.writeToLog(err_msg);
				throw std::runtime_error(err_msg);
			}
			location.upload_dir = tokens[1];
		}
		else if (directive == "index") {
			if (tokens.size() != 2) {
				std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
				_logger.writeToLog(err_msg);
				throw std::runtime_error(err_msg);
			}
			location.index = tokens[1];
		}
		else if (directive == "root") {
			if (tokens.size() != 2) {
				std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
				_logger.writeToLog(err_msg);
				throw std::runtime_error(err_msg);
			}
			location.root = tokens[1];
		}
		else if (directive == "cgi_extension") {
			if (tokens.size() != 2) {
				std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
				_logger.writeToLog(err_msg);
				throw std::runtime_error(err_msg);			}
			location.cgi_extension = tokens[1];
		}
		else if (directive == "cgi_path") {
			if (tokens.size() != 2) {
				std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
				_logger.writeToLog(err_msg);
				throw std::runtime_error(err_msg);			}
			location.cgi_path = tokens[1];
		}
		else if (directive == "return_url") {
			if (tokens.size() != 2) {
				std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
				_logger.writeToLog(err_msg);
				throw std::runtime_error(err_msg);			}
			location.return_url = tokens[1];
		}
		else if (directive == "autoindex") {
			if (tokens.size() != 2) {
				std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
				_logger.writeToLog(err_msg);
				throw std::runtime_error(err_msg);			}
			location.autoindex = _convertOnOff(tokens[1]);
		}
		else if (directive == "methods") {
			if (tokens.size() < 2) {
				std::string err_msg = ERR_CONF_WRNG_SYNTAX + directive + " on the line number " + SSTR(_line_number);
				_logger.writeToLog(err_msg);
				throw std::runtime_error(err_msg);			}
			for (size_t i = 1; i < tokens.size(); ++i){
				location.methods.push_back(tokens[i]);
			}

		}
		else {
			std::string err_msg = ERR_CONF_UNKN_DIRECTIVE + directive + " in 'location' on the line number " + SSTR(_line_number);
			_logger.writeToLog(err_msg);
			throw std::runtime_error(err_msg);

		}
	}
		std::string err_msg = ERR_CONF_BRACE_CLS + std::string("in the 'location' block but file is ended =(");
		_logger.writeToLog(err_msg);
		throw std::runtime_error(err_msg);
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
				_logger.writeToLog(err_msg);
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
	_logger.writeToLog(err_msg);
	throw std::runtime_error(err_msg);
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

