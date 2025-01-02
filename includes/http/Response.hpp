#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Message.hpp"
#include "StatusLine.hpp"

class Response : public Message<StatusLine>
{

};



#endif //RESPONSE_HPP
