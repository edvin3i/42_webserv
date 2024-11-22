#include "../../includes/logger/Logger.hpp"

Logger::Logger(LogDetail detail, LogMode mode, const std::string & logfile_name)
				:	_detail(detail), \
					_mode(mode), \
					_logFileName(logfile_name), \
					_logFile(_logFileName.c_str(), \
				std::ofstream::app) {
	if (!_logFile.is_open())
		throw std::runtime_error("ERROR: can not open the logfile!");
}

Logger::Logger()
				:	_detail(ERROR), \
					_mode(DUAL), \
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


void Logger::_writeToFile(const std::string & message) {
	_logFile << message;
}

void Logger::_writeToConsole(const std::string & message) {
	std::cout << message;
}


void Logger::writeToLog(LogDetail mode, const std::string & message) {
	_setMessagePrefix(mode);

	std::ostringstream ss;
	ss << getCurrentDateTime() << _msgPrefix;

	switch (_detail) {
		case DEBUG:
				ss << message << std::endl;
				break;
		case INFO:
			if (mode == _detail || mode == ERROR) {
				_logFile << ss.str();
				break;
			}
		// fall through
		case ERROR:
			if (mode == _detail) {
				_logFile << ss.str();
			}
	}

	switch (_mode) {
		case DUAL:
			_writeToFile(ss.str());
			_writeToConsole(ss.str());
			break;
		case LOGFILE:
			_writeToFile(ss.str());
			break;
		case CONSOLE:
			_writeToConsole(ss.str());
			break;
	}

	if (_logFile.fail())
		std::cout << "LOG out stream error!" << std::endl;
//	_logFile.flush();

}

void Logger::closeLogFile() {
	_logFile.close();
}


void Logger::_setMessagePrefix(LogDetail mode) {
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
