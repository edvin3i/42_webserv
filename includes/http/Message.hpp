#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <map>
#include <vector>
#include "../../includes/http/RequestLine.hpp"
#include "../../includes/http/StatusLine.hpp"
#include "FieldValue.hpp"
#include "Headers.hpp"
#include "Body.hpp"

class Body;

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
	Body body;
};

#endif
