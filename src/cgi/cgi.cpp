#include "cgi.hpp"

CGIHandler::CGIHandler() : _clientSocket(0) {}

CGIHandler::~CGIHandler() {}

CGIHandler &CGIHandler::operator=(const CGIHandler &other) {
	if (this != &other) {
		_clientSocket = other._clientSocket;
	}
	return (*this);
}

CGIHandler::CGIHandler(const CGIHandler &copy): _clientSocket(copy._clientSocket) {}

void	CGIHandler::cgiHandle(int clientSocket, Request r, char** env) {

	int	fd[2];
	std::string scriptName = r.path + ".php";
	std::ostringstream response;

	pipe(fd);
	int pid = fork();
	if (pid < 0){
		std::cerr << "Error: fork" << std::endl;
		close(fd[1]);
		close(fd[0]);
		buildCgiResponse(-1, response, 500);
		send(_clientSocket, response.str().c_str(), response.str().size(), 0);
		return;
	}
	else if (pid == 0) {
		std::map<std::string, std::string>::const_iterator i;
		for (i = r.header.begin(); i != r.header.end(); i++){
			std::string name;
			std::string::const_iterator it;
			for (it = i->first.begin(); it != i->first.end(); it++)
				name += toupper(*it);
			setenv(name.c_str(), i->second.c_str(), 1);
		}
		setenv("SCRIPT_FILENAME", scriptName.c_str(), 1);
        // setenv("REQUEST_METHOD", r.method, 1);
        // setenv("CONTENT_TYPE", r.type, 1);
        // setenv("CONTENT_LENGTH", r.length, 1);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		close(fd[0]);
		const char *arg[] = {"/usr/bin/php-cgi", scriptName.c_str(), NULL};
		execve("/usr/bin/php-cgi", const_cast<char *const *>(arg), env);
		std::cerr << "Error: execve" << std::endl;
		exit(1);} // autorised?
	else {
		close(fd[1]);
		waitpid(pid, 0, 0);
		buildCgiResponse(fd[0], response, 200);
		close(fd[0]);
		send(_clientSocket, response.str().c_str(), response.str().size(), 0);
	}
}


CGIHandler::CGIHandler(const CGIHandler &copy): _clientSocket(copy._clientSocket) {}

void	CGIHandler::cgiHandleBonus(int clientSocket, Request r, char** env) {

	int	fd[2];
	std::string scriptName;
	std::ostringstream response;

	if (r.method == "DELETE")
		r.path + ".py";
	else
		r.path + ".php";

	pipe(fd);
	int pid = fork();
	if (pid < 0){
		std::cerr << "Error: fork" << std::endl;
		close(fd[1]);
		close(fd[0]);
		buildCgiResponse(-1, response, 500);
		send(_clientSocket, response.str().c_str(), response.str().size(), 0);
		return;
	}
	else if (pid == 0) {
		std::map<std::string, std::string>::const_iterator i;
		for (i = r.header.begin(); i != r.header.end(); i++){
			std::string name;
			std::string::const_iterator it;
			for (it = i->first.begin(); it != i->first.end(); it++)
				name += toupper(*it);
			setenv(name.c_str(), i->second.c_str(), 1);
		}
		setenv("SCRIPT_FILENAME", scriptName.c_str(), 1);
        // setenv("REQUEST_METHOD", r.method, 1);
        // setenv("CONTENT_TYPE", r.type, 1);
        // setenv("CONTENT_LENGTH", r.length, 1);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		close(fd[0]);
		const char *arg[] = {"/usr/bin/php-cgi", scriptName.c_str(), NULL};
		execve("/usr/bin/php-cgi", const_cast<char *const *>(arg), env);
		std::cerr << "Error: execve" << std::endl;
		exit(1);} // autorised?
	else {
		close(fd[1]);
		waitpid(pid, 0, 0);
		buildCgiResponse(fd[0], response, 200);
		close(fd[0]);
		send(_clientSocket, response.str().c_str(), response.str().size(), 0);
	}
}


void	CGIHandler::buildCgiResponse(int fd, std::ostringstream &response, int num){

	char		buff[1024];
	size_t		ret;
	std::string	s;

	if (num == 500){
		response << "HTTP/1.1 500 Internal Server Error\r\n\r\n";
		return ; }
	else if (num == 200) {

		while (ret = read(fd, buff, sizeof(buff)) > 0)
			s.append(buff, ret);
    	response << "HTTP/1.1 200 OK\r\n"
				<< "Content-Type: text/html\r\n"
				<< "Content-Length: " 
       			<< s.size() << "\r\n\r\n" << s; }
}