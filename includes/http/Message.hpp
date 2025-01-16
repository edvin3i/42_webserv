#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <map>
#include <vector>
#include "FieldValue.hpp"

typedef std::multimap<std::string, FieldValue> Headers;
typedef std::pair<std::string, FieldValue> Field;

const size_t max_header_length = 1024;

template<class StartLine> struct Message
{
	StartLine start_line;
	Headers headers;
	std::string content;
	size_t content_length;
};

#endif
