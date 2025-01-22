#include "../../includes/http/Message.hpp"

template class Message<RequestLine>;
template class Message<StatusLine>;

template <class StartLine>
Message<StartLine>::Message()
: start_line(), headers(), body()
{}

template <class StartLine>
Message<StartLine>::~Message()
{}

template <class StartLine>
Message<StartLine>::Message(const Message& other)
: start_line(other.start_line), headers(other.headers), body(other.body)
{}

template <class StartLine>
Message<StartLine>& Message<StartLine>::operator=(const Message& other)
{
	if (this != &other)
	{
		start_line = other.start_line;
		headers = other.headers;
		body = other.body;
	}
	return (*this);
}


