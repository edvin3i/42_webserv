#include "../includes/Logger.hpp"

Logger::Logger(const std::string & logfile_name): _logFileName(logfile_name), \
				_logFile(_logFileName.c_str(), std::ofstream::app) {
	if (!_logFile.is_open())
		throw std::runtime_error("ERROR: can not open the logfile!");

	_logFile << "Web Server START\n";
}

Logger::Logger(): _logFileName("webserv_log_" + _getCurrentDateTimeString() + ".log"),
					_logFile(_logFileName.c_str()) {
	if (!_logFile.is_open())
		throw std::runtime_error("ERROR: can not open the logfile!");

	_logFile << "Web Server START\n";
}

std::string Logger::_getCurrentDateTimeString() {
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
	std::cout << _getCurrentDateTimeString() << std::endl;
}

Logger::~Logger() {
	_logFile.close();
}

void Logger::writeToLog(const std::string & message) {
	_logFile << _getCurrentDateTimeString() << ": " + message << std::endl;
	if (_logFile.fail())
		std::cout << "LOG out stream error!" << std::endl;
	_logFile.flush();

}

void Logger::closeLogFile() {
	_logFile.close();
}

//Logger::Logger(const Logger & other): _logFile(other.getLogFile()) {

//}

//const std::ofstream & Logger::getLogFile() const {
//	return _logFile;
//}
