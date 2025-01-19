#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <map>
#include <vector>
#include "../../includes/http/RequestLine.hpp"
#include "../../includes/http/StatusLine.hpp"
#include "FieldValue.hpp"
#include "Headers.hpp"

template<class StartLine> class Message
{
public:
	Message();
	~Message();
	Message(const Message&);
	Message& operator=(const Message&);
public:
	StartLine start_line;
	Headers headers;
	std::string content;
	size_t content_length;
};

#endif
