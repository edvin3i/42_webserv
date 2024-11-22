#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

enum LogMode {
	LOGFILE,
	CONSOLE,
	DUAL
};

enum LogDetail {
	ERROR,
	INFO,
	DEBUG
};

class Logger {
public:
	Logger(LogDetail detail, LogMode mode, const std::string & logfile_name);
	Logger();
	~Logger();
	std::string getDate() const;
	std::string getTime() const;
	std::string getCurrentDateTime() const;
	void printCurrentDateTime();
	void writeToLog(LogDetail detail, const std::string & message);
	void closeLogFile();

private:
	LogDetail _detail;
	LogMode _mode;
	std::string _logFileName;
	std::ofstream _logFile;
	std::string _msgPrefix;

	void _writeToFile(const std::string & message);
	void _writeToConsole(const std::string & message);
	void _setMessagePrefix(LogDetail mode);






};


#endif
