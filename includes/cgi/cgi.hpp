#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <unistd.h>
# include <iostream>
# include <sstream>
# include <cctype>
# include <map>
# include <sys/socket.h>
# include <sys/wait.h>

class Request;

class CGIHandler {

public:

	CGIHandler();
	~CGIHandler();
	CGIHandler &operator=(const CGIHandler &other);
	CGIHandler(const CGIHandler &copy);

	void	cgiHandle(int clientSocket, Request r, char** env);
	void	cgiHandleBonus(int clientSocket, Request r, char** env);

private:

	int		_clientSocket;
	void	buildCgiResponse(int fd, std::ostringstream &response, int num);
};

#endif
