#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>


class RequestParser {
  public:
    RequestParser();
    ~RequestParser();
    RequestParser(const RequestParser& other);
    RequestParser& operator=(const RequestParser& other);

    std::map<std::string, std::string> getHeaders() const;

  private:
     std::map<std::string, std::string> _headers;



};



#endif //REQUESTPARSER_HPP
