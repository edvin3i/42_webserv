#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <unistd.h>
# include <iostream>
# include <sstream>
# include <cctype>
# include <map>
# include <sys/socket.h>
# include <sys/wait.h>

#include "../http/response/Response.hpp"



class Response;

class CGIHandler {

public:

	CGIHandler(char **env);
	~CGIHandler();

	void	cgiHandle(Response&);
	void	cgiHandleBonus(Response&);
	bool	error() const;

private:
	CGIHandler &operator=(const CGIHandler &other);
	CGIHandler(const CGIHandler &copy);
	char	**_env;
	bool	_is_error;
	static const size_t _cgi_buffer_size;
	void _readCgi(Response&, int fd);
	void _buildCgiResponse(Response&, int fd);
	void	_setEnvironmentVariables(const Request& request, const std::string& script_path);

};

#endif
