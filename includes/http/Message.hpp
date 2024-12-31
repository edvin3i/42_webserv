#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <map>
#include <vector>

typedef std::multimap<std::string, std::string> Headers;

const size_t max_header_length = 1024;

template<class StartLine> struct Message
{
	StartLine start_line;
	Headers headers;
	std::string content;
	size_t content_length;
};

#endif
