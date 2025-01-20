#include "../../includes/cgi/Cgi.hpp"

const size_t CGIHandler::_buffer_size = 1024;

CGIHandler::CGIHandler(char **env) : _is_error(false), _env(env) {}

CGIHandler::~CGIHandler() {}

CGIHandler &CGIHandler::operator=(const CGIHandler &other) {
	if (this != &other) {
		_is_error = other._is_error;
	}
	return (*this);
}

CGIHandler::CGIHandler(const CGIHandler &copy): _is_error(copy._is_error) {}

void CGIHandler::_setEnvironmentVariables(const Request& request, const std::string& script_path)
{
	const Headers& headers = request.getHeaders();



}

void	CGIHandler::cgiHandle(Response& response)
{

}

void CGIHandler::_buildCgiResponse(Response& response, int fd)
{
	_readCgi(response, fd);
	response.setStatusLine(StatusLine(STATUS_OK));
	response.headers_insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue(Utils::size_t_to_str(response.getContentLength()))));
	response.headers_insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_TYPE), FieldValue(MimeType::get_mime_type("html"))));
}

void CGIHandler::_readCgi(Response& response, int fd)
{
	char buffer[_buffer_size];
	ssize_t ret;

	while (true)
	{
		ret = read(fd, buffer, sizeof(buffer));

		if (ret < 0)
			throw (std::exception());
		if (ret == 0)
			return ;
		if (ret > 0)
		{
			response.content_append(buffer, ret);
			response.content_length_add(ret);
		}
	}
}

bool CGIHandler::error() const
{
	return (_is_error);
}
