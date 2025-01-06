#ifndef ERRORPAGES_HPP
#define ERRORPAGES_HPP

#include <map>
#include <string>
#include <sstream>

#define WEBSERV_VER "0.00.001"

class ErrorPages {
public:
  static std::string getErrorPage(int code);

private:
  static std::map<int, std::string> _errorPages;

  static void _initErrorPages();

};



#endif //ERRORPAGES_HPP
