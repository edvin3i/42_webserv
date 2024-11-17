#include "../includes/Logger.hpp"

Logger::Logger(const std::string & logfile_name): _logFileName(logfile_name), \
				_logFile(_logFileName.c_str(), std::ofstream::app) {
	if (!_logFile.is_open())
		throw std::runtime_error("ERROR: can not open the logfile!");

	_logFile << "Web Server START\n";
}

Logger::Logger(): _logFileName("webserv_log_" + _getCurrentDateTimeString() + ".log"), _logFile(_logFileName.c_str()) {
	if (!_logFile.is_open())
		throw std::runtime_error("ERROR: can not open the logfile!");

	_logFile << "Web Server START\n";
}

std::string Logger::_getCurrentDateTimeString() {
	time_t currentTime;
	struct tm *localTime;
	std::string dateTimeString;

	time(& currentTime);
	localTime = localtime(& currentTime);

	dateTimeString = "[" + SSTR(localTime->tm_year + 1900) + "." + SSTR(localTime->tm_mon + 1) \
 			+ "." + SSTR(localTime->tm_mday) + "]_[" + SSTR(localTime->tm_hour) + ":" \
 			+ SSTR(localTime->tm_min) + ":" + SSTR(localTime->tm_sec) + "]";

	return dateTimeString;
}

void Logger::printCurrentDateTime() {
	std::cout << _getCurrentDateTimeString() << std::endl;
}

Logger::~Logger() {
	_logFile.close();
}

void Logger::writeToLog(const std::string &message) {
	_logFile << _getCurrentDateTimeString() << ": " + message << std::endl;

}
