#ifndef BODYPART_HPP
#define BODYPART_HPP

#include <string>
#include "Message.hpp"

class BodyPart
{
public:
	BodyPart(const std::string& str);
	~BodyPart();
	BodyPart(const BodyPart&);
	BodyPart& operator=(const BodyPart&);

private:
	BodyPart();
	void _parse_body(const std::string&);
	void _split_body_part(const std::string& str, std::vector<std::string> & fields);
	void _check_headers();
private:
	Headers _headers;
	std::string _body;
};

#endif
