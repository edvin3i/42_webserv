#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

class Logger {
public:
	Logger(const std::string & logfile_name);
	Logger();
	//Logger(const Logger & other);
	~Logger();
	void printCurrentDateTime();
	void writeToLog(const std::string & message);
	//const std::ofstream & getLogFile() const;
	void closeLogFile();

private:
	std::string _logFileName;
	std::ofstream _logFile;
	std::string _getCurrentDateTimeString();





};


#endif
