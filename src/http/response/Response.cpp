#include "../../../includes/http/response/Response.hpp"

void Response::_init_status_code_message()
{
	_status_code_message[STATUS_OK] = "OK";
	_status_code_message[STATUS_CREATED] = "Created";
	_status_code_message[STATUS_NO_CONTENT] = "No Content";
	_status_code_message[STATUS_MOVED] = "Moved Permanently";
	_status_code_message[STATUS_BAD_REQUEST] = "Bad Request";
	_status_code_message[STATUS_FORBIDDEN] = "Forbidden";
	_status_code_message[STATUS_NOT_FOUND] = "Not Found";
	_status_code_message[STATUS_NOT_ALLOWED] = "Method Not Allowed";
	_status_code_message[STATUS_CONFLICT] = "Conflict";
	_status_code_message[STATUS_TOO_LARGE] = "Payload Too Large";
	_status_code_message[STATUS_URI_TOO_LONG] = "URI Too Long";
	_status_code_message[STATUS_INTERNAL_ERR] = "Internal Server Error";
}

Response::Response(const Request& request)
{

}

const std::string& Response::str() const
{
	return (_str_content);
}
