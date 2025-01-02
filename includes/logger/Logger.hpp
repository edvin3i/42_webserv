#ifndef LOGGER_HPP
#define LOGGER_HPP

#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define RESET "\033[0m"
#define BRIGHT_BLACK   "\033[90m"
#define BRIGHT_RED     "\033[91m"
#define BRIGHT_GREEN   "\033[92m"
#define BRIGHT_YELLOW  "\033[93m"
#define BRIGHT_BLUE    "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN    "\033[96m"
#define BRIGHT_WHITE   "\033[97m"
#define BG_BRIGHT_BLACK   "\033[100m"
#define BG_BLUE "\033[44m"
#define BG_YELLOW "\033[43m"
#define BG_BRIGHT_RED "\033[101m"
#define BG_BRIGHT_YELLOW "\033[103m"
#define BG_BRIGHT_GREEN "\033[102m"


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
	static Logger &get_logger(LogDetail detail, LogMode mode, const std::string & logfile_name);
	static Logger *get_ptr();
	~Logger();
	std::string getDate() const;
	std::string getTime() const;
	std::string getCurrentDateTime() const;
	void printCurrentDateTime();
	void writeToLog(LogDetail detail, const std::string & message);
	void closeLogFile();

private:
	Logger(LogDetail detail, LogMode mode, const std::string & logfile_name);
	Logger();
	Logger(const Logger &other);
	Logger &operator=(const Logger & other);
	LogDetail _detail;
	LogMode _mode;
	std::string _logFileName;
	std::ofstream _logFile;
	std::string _msgPrefix;
	static Logger *_logger_ptr;

	void _writeToFile(const std::string & message);
	void _writeToConsole(const std::string & message);
	void _setMessagePrefix(LogDetail mode);






};


#endif
