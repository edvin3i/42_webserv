#include "../../includes/http/RequestParser.hpp"


std::string RequestParser::_getSubStrCharsVec(std::vector<char> & vec, char end) {
	return std::string();
}


RequestParser::RequestParser() {
}

RequestParser::~RequestParser() {
}

void RequestParser::parse(std::vector<char> & inputVector) {
	// convert vector to string (more comfortable parse string instead vector)
	std::string inStr(inputVector.begin(), inputVector.end());

	// extract whole request
	size_t startPos = 0;
	size_t endPos = inStr.find("\r\n\r\n");
	std::string fullRequest = inStr.substr(startPos, endPos);

	// extract and parse first request line
	endPos = fullRequest.find("\r\n");
	std::string startLine = inStr.substr(0, endPos);
	_parseStartLine(startLine);
	//fullRequest.erase(0, endPos);

	// extract and parse header
	startPos = endPos;
	endPos = fullRequest.find("\r\n");
	std::string headerLine = fullRequest.substr(startPos, endPos);
	_parseHeader(headerLine);

	// extract and parse request body
	startPos = endPos;
	endPos = fullRequest.find("\r\n\r\n");
	std::string requestBody = fullRequest.substr(startPos, endPos);
	_parseMessageBody(requestBody);

}

void RequestParser::_parseStartLine(std::string & string) {
	std::cout << "====== First line ======" << std::endl;
	std::cout << string << std::endl;
	std::cout << "========= END ==========\n" << std::endl;

}

void RequestParser::_parseHeader(std::string &string) {
	std::cout << "====== Header line ======" << std::endl;
	std::cout << string << std::endl;
	std::cout << "========= END ==========\n" << std::endl;
}

void RequestParser::_parseMessageBody(std::string &string) {
	std::cout << "========= Request body ==========" << std::endl;
	std::cout << string << std::endl;
	std::cout << "========= END ==========\n" << std::endl;
}

