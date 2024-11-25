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
	//size_t startPos = 0;
	size_t endPos = inStr.find("\r\n\r\n");
	std::string fullRequest = inStr.substr(0, endPos);

	std::cout << "====== Full request BEFORE ======" << std::endl;
	std::cout << fullRequest << std::endl;
	std::cout << "========= END ==========\n" << std::endl;

	// extract and parse first request line
	endPos = fullRequest.find("\r\n");
	std::string startLine = inStr.substr(0, endPos);
	_parseStartLine(startLine);
	fullRequest.erase(0, startLine.size());

	// extract and parse header
	//startPos = endPos;
	endPos = fullRequest.find("\r\n");
	std::string headerLine = fullRequest.substr(0, endPos);
	_parseHeader(headerLine);
	fullRequest.erase(0, headerLine.size());

	// extract and parse request body
	//startPos = endPos;
	endPos = fullRequest.find("\r\n\r\n");
	std::string requestBody = fullRequest.substr(0, endPos);
	_parseMessageBody(requestBody);
	fullRequest.erase(0, std::string::npos);

	std::cout << "====== Full request AFTER ======" << std::endl;
	std::cout << fullRequest << std::endl;
	std::cout << "========= END ==========\n" << std::endl;

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

