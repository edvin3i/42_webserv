#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()



class Logger {
public:
	Logger(const std::string &logfile_name);
	Logger();
	~Logger();
	void printCurrentDateTime();
	void writeToLog(const std::string & message);

private:
	std::string _logFileName;
	std::ofstream _logFile;
	std::string _getCurrentDateTimeString();





};


#endif
