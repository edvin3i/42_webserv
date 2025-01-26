#include "../../includes/logger/Logger.hpp"

Logger *Logger::_logger_ptr = NULL;

Logger::Logger(LogDetail detail, LogMode mode, const std::string & logfile_name)
				:	_detail(detail), \
					_mode(mode), \
					_logFileName(logfile_name) {

	if (_mode != CONSOLE) {
		_logFile.open(_logFileName.c_str(), std::ofstream::app);
		if (!_logFile.is_open())
			throw std::runtime_error("ERROR: can not open the logfile!");
	}
}

Logger::Logger()
				:	_detail(ERROR), \
					_mode(DUAL), \
					_logFileName("webserv_log_" \
					+ getCurrentDateTime() + ".log"),
					_logFile() {

	if (_mode != CONSOLE) {
		_logFile.open(_logFileName.c_str(), std::ofstream::app);
		if (!_logFile.is_open())
			throw std::runtime_error("ERROR: can not open the logfile!");
	}
}


Logger &Logger::get_logger(LogDetail detail, LogMode mode,
                           const std::string &logfile_name) {
	if (_logger_ptr == NULL) {
		_logger_ptr = new Logger(detail, mode, logfile_name);
	}
	else {
		std::cerr << "Logger instance is created earlier!" << std::endl;
		std::cerr << "Previous reference will be returned." << std::endl;
	}

	return *_logger_ptr;
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


void Logger::_writeToFile(const std::string & message) {
	_logFile << message;
}

void Logger::_writeToConsole(const std::string & message) {
	std::cout << message;
}


void Logger::writeToLog(LogDetail detail, const std::string & message) {
	if (detail > _detail) {
		return;
	}

	_setMessagePrefix(detail);
	std::ostringstream ss;
	ss << getCurrentDateTime() << _msgPrefix;
	ss << message << std::endl;

	/*
	 * Choose the way to write log message
	 */
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
	_logFile.flush();
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


void Logger::closeLogFile() {
	_logFile.close();
}

Logger * Logger::get_ptr() {
	return _logger_ptr;
}

//Logger::Logger(const Logger & other): _logFile(other.getLogFile()) {

//}

//const std::ofstream & Logger::getLogFile() const {
//	return _logFile;
//}
