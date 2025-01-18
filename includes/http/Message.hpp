#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <map>
#include <vector>
#include "FieldValue.hpp"
#include "Headers.hpp"

template<class StartLine> struct Message
{
	StartLine start_line;
	Headers headers;
	std::string content;
	size_t content_length;
};

#endif
