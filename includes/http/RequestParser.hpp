#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>


class RequestParser {
  public:
    RequestParser();
    ~RequestParser();
    RequestParser(const RequestParser & other);
    RequestParser& operator=(const RequestParser & other);
	void parse(std::vector<char> & inputVector);

    std::map<std::string, std::string> getHeaders() const;

  private:
	std::map<std::string, std::string> _start;
	std::map<std::string, std::string> _header;
	std::vector<char> _reqBody;

    void _parseStartLine(std::string & string);
    void _parseHeader(std::string & string);
	void _parseMessageBody(std::string & string);

	std::string _getSubStrCharsVec(std::vector<char> & vec, char end);
};



#endif //REQUESTPARSER_HPP
