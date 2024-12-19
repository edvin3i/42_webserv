#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <map>
#include <vector>

typedef std::map<std::string, std::vector<std::string> > Headers;

template<class StartLine> struct Message
{
	StartLine start_line;
	Headers headers;
	std::string content;
	size_t content_length;
};

#endif
