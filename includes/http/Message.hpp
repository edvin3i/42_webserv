#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <map>

typedef std::map<std::string, std::string> Headers;

template<class StartLine> struct Message
{
	StartLine start_line;
	Headers headers;
	std::string body;
	size_t body_size;
};

#endif
