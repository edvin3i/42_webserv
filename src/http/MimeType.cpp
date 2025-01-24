#include "../../includes/http/MimeType.hpp"

bool MimeType::_mime_type_init = false;

const std::string MimeType::default_mime_type = "application/octet-stream";

std::map<std::string, std::string> MimeType::_mime_type;

void MimeType::init_mime_type()
{
	if (_mime_type_init)
		return ;
	_mime_type["html"] = "text/html";
	_mime_type["css"] = "text/css";
	_mime_type["js"] = "application/javascript";
	_mime_type["jpg"] = "image/jpeg";
	_mime_type["png"] = "image/png";
	_mime_type["gif"] = "image/gif";
	_mime_type["pdf"] = "application/pdf";
	_mime_type["txt"] = "text/plain";
	_mime_type["xml"] = "application/xml";
	_mime_type["json"] = "application/json";
	_mime_type["mp4"] = "video/mp4";
	_mime_type["mp3"] = "audio/mpeg";
	_mime_type_init = true;
}

const std::string& MimeType::get_mime_type(const std::string& extension)
{
	try
	{
		const std::string& type = _mime_type.at(extension);
		return (type);
	}
	catch (...)
	{
		throw (STATUS_UNSUPPORTED_MEDIA_TYPE);
	}
}
