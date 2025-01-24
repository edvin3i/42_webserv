#ifndef MIMETYPE_HPP
#define MIMETYPE_HPP

#include <string>
#include <map>
#include "StatusCode.hpp"

class MimeType
{
public:
	static void init_mime_type();
	static const std::string& get_mime_type(const std::string& extension);
	static const std::string default_mime_type;
private:
	MimeType();
	~MimeType();
	MimeType(const MimeType&);
	MimeType& operator=(const MimeType&);
	static std::map<std::string, std::string> _mime_type;
	static bool _mime_type_init;
};

#endif
