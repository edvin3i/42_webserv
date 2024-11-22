#include "../../includes/logger/Logger.hpp"

Logger::Logger(LogMode mode, const std::string & logfile_name)
				:	_mode(mode), \
					_logFileName(logfile_name), \
					_logFile(_logFileName.c_str(), \
				std::ofstream::app) {
	if (!_logFile.is_open())
		throw std::runtime_error("ERROR: can not open the logfile!");
}

Logger::Logger()
				:	_mode(ERROR), \
					_logFileName("webserv_log_" \
					+ getCurrentDateTime() + ".log"),
					_logFile(_logFileName.c_str()) {
	if (!_logFile.is_open())
		throw std::runtime_error("ERROR: can not open the logfile!");
}





std::string Logger::getCurrentDateTime() const {
	time_t currentTime;
	struct tm *localTime;

	time(& currentTime);
	localTime = localtime(& currentTime);

	std::ostringstream ss;
	ss << "["
		<< (localTime->tm_year + 1900) << "." \
		<< (localTime->tm_mon + 1) << "." \
		<< localTime->tm_mday << "]_[" \
		<< localTime->tm_hour << ":" \
		<< localTime->tm_min << ":" \
		<< localTime->tm_sec \
		<< "]";

	return ss.str();
}

void Logger::printCurrentDateTime() {
	std::cout << getCurrentDateTime() << std::endl;
}

Logger::~Logger() {
	_logFile.close();
}

std::string Logger::getDate() const {
	time_t currentTime;
	struct tm *localTime;

	time(& currentTime);
	localTime = localtime(& currentTime);

	std::ostringstream ss;
	ss << "["
		<< (localTime->tm_year + 1900) << "." \
		<< (localTime->tm_mon + 1) << "." \
		<< localTime->tm_mday << "]";
	return ss.str();
}

std::string Logger::getTime() const {
	time_t currentTime;
	struct tm *localTime;

	time(& currentTime);
	localTime = localtime(& currentTime);

	std::ostringstream ss;
	ss << "[" \
		<< localTime->tm_hour << ":" \
		<< localTime->tm_min << ":" \
		<< localTime->tm_sec \
		<< "]";

	return ss.str();
}

void Logger::writeToLog(LogMode mode, const std::string & message) {
	_setMessagePrefix(mode);

	std::ostringstream ss;
	ss << getCurrentDateTime() << _msgPrefix;

	switch (_mode) {
		case ERROR:
			if (mode == _mode) {
				ss << message << std::endl;
				break;
			}
			// fall through
		case INFO:
			if (mode == _mode || mode == ERROR) {
				ss << message << std::endl;
				break;
			}
			// fall through
		case DEBUG:
			ss << message << std::endl;
			break;
	}
		_logFile << ss.str();

	if (_logFile.fail())
		std::cout << "LOG out stream error!" << std::endl;
	_logFile.flush();

}

void Logger::closeLogFile() {
	_logFile.close();
}

void Logger::_setMessagePrefix(LogMode mode) {
	switch (mode) {
		case INFO:
			_msgPrefix = " INFO: ";
			break;
		case ERROR:
			_msgPrefix = " ERROR: ";
			break;
		case DEBUG:
			_msgPrefix = " DEBUG: ";
	}
}


//Logger::Logger(const Logger & other): _logFile(other.getLogFile()) {

//}

//const std::ofstream & Logger::getLogFile() const {
//	return _logFile;
//}
