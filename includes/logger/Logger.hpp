#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

enum LogMode {
	ERROR,
	INFO,
	DEBUG
};

class Logger {
public:
	Logger(LogMode mode, const std::string & logfile_name);
	Logger();
	//Logger(const Logger & other);
	~Logger();
	std::string getDate() const;
	std::string getTime() const;
	std::string getCurrentDateTime() const;
	void printCurrentDateTime();
	void writeToLog(LogMode mode, const std::string & message);
	//const std::ofstream & getLogFile() const;
	void closeLogFile();

private:
	LogMode _mode;
	std::string _logFileName;
	std::ofstream _logFile;
	std::string _msgPrefix;

	void _setMessagePrefix(LogMode mode);






};


#endif
