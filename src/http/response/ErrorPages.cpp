#include "../../../includes/http/response/ErrorPages.hpp"

std::map<int, std::string> ErrorPages::_errorPages;

std::string ErrorPages::getErrorPage(int code) {
	std::ostringstream ss;

	ss	<< "HTTP/1.1 " << code << " " << _errorPages[code];
	ss  << "\nContent-Type: text/html\nContent-Length: \r\n\r\n"; // need to add content-length
	ss	<< "<!DOCTYPE html>\n";
	ss	<< "<html>\n";
	ss	<< "<head>\n";
	ss	<< "<title>" << code << " " << _errorPages[code] << "</title>\n";
	ss	<< "</head>\n";
	ss	<< "<body>\n";
	ss	<< "<h1>" << code << " " << _errorPages[code] << "</h1>\n";
	ss	<< "<hr>\n";
	ss	<< "<hr><center>WebServ42/" << WEBSERV_VER << "</center>";
	ss	<< "</body>\n";
	ss	<< "</html>\n";
	ss	<< "\r\n\r\n";

	return ss.str();
}

void ErrorPages::_initErrorPages() {
// 4XX pages
	_errorPages[400] = "Bad Request";
	_errorPages[401] = "Unauthorized";
	_errorPages[403] = "Forbidden";
	_errorPages[404] = "Not Found";
	_errorPages[405] = "Method Not Allowed";
	_errorPages[406] = "Not Acceptable";
// 5XX pages
	_errorPages[500] = "Internal Server Error";
	_errorPages[501] = "Not Implemented";
	_errorPages[502] = "Bad Gateway";
	_errorPages[503] = "Service Unavailable";
	_errorPages[504] = "Gateway Timeout";
	_errorPages[505] = "HTTP Version Not Supported";

}
