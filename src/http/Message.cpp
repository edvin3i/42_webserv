#include "../../includes/http/Message.hpp"

template class Message<RequestLine>;
template class Message<StatusLine>;

template <class StartLine>
Message<StartLine>::Message()
: start_line(), headers(), content(), content_length(0)
{}

template <class StartLine>
Message<StartLine>::~Message()
{}

template <class StartLine>
Message<StartLine>::Message(const Message& other)
: start_line(other.start_line), headers(other.headers), content(other.content), content_length(other.content_length)
{}

template <class StartLine>
Message<StartLine>& Message<StartLine>::operator=(const Message& other)
{
	if (this != &other)
	{
		start_line = other.start_line;
		headers = other.headers;
		content = other.content;
		content_length = other.content_length;
	}
	return (*this);
}


