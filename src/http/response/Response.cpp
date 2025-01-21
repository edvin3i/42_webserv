#include "../../../includes/http/response/Response.hpp"

const std::string Response::_html_auto_index = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Directory Index</title><style>body{font-family:'Arial',sans-serif;background-color:#f8f8f8;margin:0;padding:20px;}h1{color:#333;}ul{list-style-type:none;padding:0;}li{margin:10px 0;}a{color:#007bff;text-decoration:none;}a:hover{text-decoration:underline;}</style></head><body><h1>Index of The Directory</h1><ul>";

const std::string Response::_default_error_page_path = "www/website/error_pages/default_error.html";

const size_t Response::_cgi_buffer_size = 1024;

Response::Response(Logger & logger, const ServerConfig & conf, const LocationConfig  *location, const Request & request, Env env)
: _logger(logger), _request(request), _conf(conf), _location(location),
  _str_content(), _resource_path(), _resource_type(), _has_index_file(false),
  _index_file(), _env(env)
{
	if (_request.error())
	{
		_handle_error(_request.getErrorCode());
		return ;
	}
	try
	{
		if (_check_redirect())
		{
			_handle_redirect();
			return ;
		}
		_check_method_allowed();
		_check_resource();
		if (_check_cgi_extension())
		{
			_execute_cgi();
			return ;
		}
		switch (_request.getStartLine().getMethod().getValue())
		{
			case METHOD_GET:
				_handle_get();
				break ;
			case METHOD_POST:
				_handle_post();
				break;
			case METHOD_DELETE:
				_handle_delete();
				break ;
		}
	}
	catch (enum e_status_code status_code)
	{
		_handle_error(status_code);
	}
}


Response::~Response()
{}

bool Response::_check_redirect() const
{
	return(_location && !_location->return_url.empty());
}

void Response::_handle_redirect()
{
	start_line = StatusLine(STATUS_MOVED);
	headers.insert(SingleField(Headers::getTypeStr(HEADER_LOCATION), FieldValue(_location->return_url)));
	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue("0")));
	content_length = 0;
}

void Response::_check_method_allowed()
{
	if (_location)
	{
		const std::vector<std::string>& allow_methods = _location->methods;
		const std::string& request_method = _request.getStartLine().getMethod().toString();
		if (std::find(allow_methods.begin(), allow_methods.end(), request_method) == allow_methods.end())
			throw (STATUS_NOT_ALLOWED);
	}
}


void Response::_check_location()
{
	// else if (redirection)
	// {
	// 	redirection ?
	// }

}

void Response::_check_resource()
{
	struct stat file_stat;
	std::string resource_path;
	const std::string& uri_path = _request.getStartLine().getUri().getPath();

	if (_request.getStartLine().getMethod().toString() == "POST")
	{
		if (!_location || _location->upload_dir.empty())
			throw (STATUS_INTERNAL_ERR);
	}
	if (_location && !_location->path.empty())
	{
		if (!_location->root.empty())
			_resource_path.append(_location->root);
		else
			_resource_path.append(_conf.root);
		_resource_path.append(_location->path);
		_resource_path.append(uri_path.substr(_location->path.length()));
	}
	else
	{
		_resource_path.append(_conf.root);
		_resource_path.append(uri_path);
	}
	if (stat(_resource_path.c_str(), &file_stat) < 0)
	{
		throw (STATUS_NOT_FOUND);
	}
	else if (S_ISDIR(file_stat.st_mode)) {
		std::ostringstream ss; ss << "Resource is a directory" << std::endl;
		_logger.writeToLog(DEBUG, ss.str());
		ss.str("");
		_resource_type = RT_DIR;
		if (_resource_path[_resource_path.length() - 1] != '/')
			_resource_path.push_back('/');
		_check_index_file();
	}
	else {
		std::ostringstream ss; ss << "Resource is a file" << std::endl;
		_logger.writeToLog(DEBUG, ss.str());
		ss.str("");
		_resource_type = RT_FILE;
	}
}

bool Response::_check_cgi_extension() const
{
	std::string filepath;

	switch (_resource_type)
	{
		case RT_FILE:
			filepath = _resource_path;
			break ;
		case RT_DIR:
			if (!_has_index_file)
				return (false);
			filepath = _resource_path + _index_file;
			break ;
		default:
			throw (STATUS_INTERNAL_ERR);
	}

	if (!_location || _location->cgi_extension.empty())
		return (false);

	size_t dot_pos = filepath.find_last_of('.');

	if (dot_pos == std::string::npos || (dot_pos == filepath.length() - 1))
		return (false);
	if (filepath.substr(dot_pos + 1) != _location->cgi_extension)
		return (false);
	return (true);
}

void Response::_setEnvironmentVariables()
{
	const Headers& request_headers = _request.getHeaders();

	_env.setEnv("REDIRECT_STATUS", "200");

	_env.setEnv("REQUEST_METHOD", _request.getStartLine().getMethod().toString());

	Headers::const_iterator content_type_it = request_headers.find(Headers::getTypeStr(HEADER_CONTENT_TYPE));

	if (content_type_it != request_headers.end())
		_env.setEnv("CONTENT_TYPE", content_type_it->second.getValue());
	else
		_env.setEnv("CONTENT_TYPE", "");

	Headers::const_iterator content_length_it = request_headers.find(Headers::getTypeStr(HEADER_CONTENT_LENGTH));

	if (content_length_it != request_headers.end())
		_env.setEnv("CONTENT_LENGTH", content_length_it->second.getValue());
	else
		_env.setEnv("CONTENT_LENGTH", "");
	std::string filepath = _resource_path;

	if (_resource_type == RT_DIR && _has_index_file)
		filepath.append(_index_file);

	_env.setEnv("SCRIPT_FILENAME", filepath);

	_env.setEnv("SERVER_PROTOCOL", "HTTP/1.1");

	_env.setEnv("GATEWAY_INTERFACE", "CGI/1.1");

	const std::string& query =_request.getStartLine().getUri().getQuery().c_str();

	_env.setEnv("QUERY_STRING", query);
}

void Response::_buildCgiResponse(const std::string& cgi_output)
{
	std::string cgi_content;
	Headers cgi_headers;
	_parse_cgi(cgi_output, cgi_content, cgi_headers);
	content = cgi_content;
	content_length = cgi_content.length();

	Headers::const_iterator content_type_it, content_length_it;

	content_type_it = cgi_headers.find(Headers::getTypeStr(HEADER_CONTENT_TYPE));
	content_length_it = cgi_headers.find(Headers::getTypeStr(HEADER_CONTENT_LENGTH));

	if (content_type_it == cgi_headers.end())
		throw (STATUS_INTERNAL_ERR);
	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_TYPE), FieldValue(content_type_it->second.getValue())));

	if (content_length_it != cgi_headers.end())
		headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue(content_length_it->second.getValue())));
	else
		headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue(Utils::size_t_to_str(content_length))));
	start_line = StatusLine(STATUS_OK);
}

void Response::_parse_cgi(const std::string& cgi_output, std::string& cgi_content, Headers& cgi_headers)
{
	size_t header_end = cgi_output.find("\r\n\r\n");

	if (header_end != std::string::npos)
	{
		std::vector<std::string> fields;
		std::string cgi_headers_str = cgi_output.substr(0, header_end);
		cgi_content = cgi_output.substr(header_end + 4);

		fields = Utils::split(cgi_headers_str, "\r\n");
		cgi_headers = Headers(fields);
	}
}

void Response::_readCgi(int fd, std::string& str)
{
	char buffer[_cgi_buffer_size];
	ssize_t ret;

	while (true)
	{
		ret = read(fd, buffer, sizeof(buffer));

		if (ret < 0)
			throw (STATUS_INTERNAL_ERR);
		if (ret == 0)
			return ;
		if (ret > 0)
			str.append(buffer, ret);
	}
}

bool Response::_check_cgi_path() const
{
	struct stat file_stat;
	if (_location->cgi_path.empty())
		return (false);
	if (stat(_location->cgi_path.c_str(), &file_stat) != 0)
		return (false);
	if (!S_ISREG(file_stat.st_mode))
		return (false);
	if (access(_location->cgi_path.c_str(), X_OK) != 0)
		return (false);
	return (true);
}

void Response::_sendCgi(int fd)
{
	if (!_request.getContent().empty())
	{
		ssize_t written = write(fd, _request.getContent().c_str(), _request.getContent().length());
		if (written < 0)
		{
			close (fd);
			throw (STATUS_INTERNAL_ERR);
		}
	}
}

void Response::_execute_cgi()
{
	if (!_check_cgi_path())
		throw (STATUS_INTERNAL_ERR);

	int	fd[2];
	_setEnvironmentVariables();
	char **env = _env.toArray();

	pipe(fd);
	int pid = fork();
	if (pid < 0){
		std::cerr << "Error: fork" << std::endl;
		close(fd[1]);
		close(fd[0]);
		throw (STATUS_INTERNAL_ERR);
		return ;
	}
	else if (pid == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		dup2(fd[0], STDIN_FILENO);
		close(fd[1]);
		close(fd[0]);
		const char *arg[] = {_location->cgi_path.c_str(), _resource_path.c_str(), NULL};
		execve(_location->cgi_path.c_str(), const_cast<char *const *>(arg), env);
		std::cerr << "Error: execve" << std::endl;
		exit(EXIT_FAILURE);} // autorised?
	else
	{
		if (_request.getStartLine().getMethod().getValue() == METHOD_POST)
			_sendCgi(fd[1]);
		close(fd[1]);

		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
		{
			int exit_code = WEXITSTATUS(status);
			if (exit_code != 0)
				throw (STATUS_INTERNAL_ERR);
		}
		else if (WIFSIGNALED(status))
			throw (STATUS_INTERNAL_ERR);

		std::string cgi_output;

		_readCgi(fd[0], cgi_output);
		close(fd[0]);

		_buildCgiResponse(cgi_output);
	}
	Env::freeArray(env);
}

void Response::_handle_get()
{
	switch (_resource_type)
	{
		case RT_FILE:
			_handle_file(_resource_path);
			break ;
		case RT_DIR:
			_handle_dir();
			break;
		default:
			throw (STATUS_INTERNAL_ERR);
	}
}

static char my_tolower(char c)
{
	return (static_cast<char>(tolower(c)));
}

static const std::string& _filename_to_mime_type(const std::string& filename)
{
	const size_t dot_pos = filename.find_last_of(".");
	if (dot_pos == std::string::npos || dot_pos == filename.length() - 1)
		return (MimeType::default_mime_type);

	const std::string extension = filename.substr(dot_pos + 1);
	std::string lower_extension;
	std::transform(extension.begin(), extension.end(), std::back_inserter(lower_extension), my_tolower);
	return (MimeType::get_mime_type(lower_extension));
}

void Response::_handle_file(const std::string& filepath)
{
	std::ifstream file(filepath.c_str(), std::ios::binary);
	std::stringstream file_content;
	// std::string file_length_str;
	// std::string file_extension;

	if (!file.is_open())
		throw (STATUS_INTERNAL_ERR);

	// getting filesize
	file.seekg(0, std::ios::end);
	std::streamsize file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	// reading file
	file_content << file.rdbuf();

	start_line = StatusLine(STATUS_OK);
	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue(Utils::size_t_to_str(file_content.str().length()))));
	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_TYPE), FieldValue(_filename_to_mime_type(filepath))));

  // 	headers.insert(Field(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue(size_t_to_str(file_size))));
  // 	headers.insert(Field(Headers::getTypeStr(HEADER_CONTENT_TYPE), FieldValue(_filename_to_mime_type(filename))));
	content = file_content.str();
	content_length = file_size;
}

void Response::_handle_dir()
{
	std::string filepath;

	if (_has_index_file)
	{
		filepath.append(_resource_path);
		filepath.append(_index_file);
		_handle_file(filepath);
	}
	else if (_check_auto_index())
		_handle_auto_index();
	else
		throw (STATUS_FORBIDDEN);
}

void Response::_check_index_file()
{
	DIR *dir = opendir(_resource_path.c_str());
	struct dirent *file;
	std::string index_file;

	if (_location && !_location->index.empty())
		index_file = _location->index;
	else
		index_file = _conf.index;

	if (!dir)
		return ;
	while ((file = readdir(dir)))
	{
		if (strcmp(file->d_name, index_file.c_str()) == 0)
		{
			_index_file = index_file;
			_has_index_file = true;
			break ;
		}
	}
	closedir(dir);
}

// bool Response::_is_dir_has_index_file()
// {
// 	DIR *dir = opendir(_resource_path.c_str());
// 	struct dirent *file;
// 	bool index_found = false;
// 	std::string index_file;

// 	if (_location && !_location->index.empty())
// 		index_file = _location->index;
// 	else
// 		index_file = _conf.index;

// 	if (!dir)
// 		return (false);
// 	while ((file = readdir(dir)))
// 	{
// 		if (strcmp(file->d_name, index_file.c_str()) == 0)
// 		{
// 			index_found = true;
// 			break ;
// 		}
// 	}
// 	closedir(dir);
// 	return (index_found);
// }

bool Response::_check_auto_index()
{
	return (_location && _location->autoindex);
}

void Response::_handle_auto_index()
{
	DIR *dir = opendir(_resource_path.c_str());
	struct dirent* file;

	if (!dir)
		throw (STATUS_INTERNAL_ERR);
	content.append(_html_auto_index);
	while ((file = readdir(dir)))
	{
		std::string filename(file->d_name);
		if (filename != "." && filename != "..") {
			content.append("<li><a href=\"");
			content.append(_request.getStartLine().getUri().getPath());
			const std::string& path = _request.getStartLine().getUri().getPath();
			if (!path.empty() && path[path.length() - 1] != '/')
				content.append("/");
			content.append(filename);
			content.append("\">");
			content.append(filename);
			content.append("</a></li>");
		}
	}
	content.append("</ul></body></html>");
	closedir(dir);

	start_line = StatusLine(STATUS_OK);
	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue(Utils::size_t_to_str(content.length()))));
	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_TYPE), FieldValue(MimeType::get_mime_type("html"))));
	content_length = content.length();
}

void Response::_check_body_size() {
	// if request has body
	if (_request.getContentLength() > 0) {
		// compare with server_config limit
		if (_conf.client_max_body_size > 0 && _request.getContentLength() > _conf.client_max_body_size) {
			_logger.writeToLog(ERROR, "Request body size exceeds client_max_body_size limit");
			throw (STATUS_TOO_LARGE);
		}
	}
}

void Response::_handle_multipart_datas()
{
	const std::vector<BodyPart>& multipart = _request.getMultipart();

	if (multipart.empty())
		throw (STATUS_BAD_REQUEST);

	size_t count_file_uploaded = 0;
	for (size_t i = 0; i < multipart.size(); ++i)
	{
		_handle_multipart_data(multipart[i], count_file_uploaded);
	}
	if (count_file_uploaded == 0)
		throw (STATUS_INTERNAL_ERR);
	start_line = StatusLine(STATUS_CREATED);
	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue("0")));
}

void Response::_handle_multipart_data(const BodyPart& body_part, size_t& count)
{
	const Headers& bodypart_headers = body_part.getHeaders();
	Headers::const_iterator content_disposition_it = bodypart_headers.find(Headers::getTypeStr(HEADER_CONTENT_DISPOSITION));

	if (bodypart_headers.count(Headers::getTypeStr(HEADER_CONTENT_DISPOSITION)) == 0)
		throw (STATUS_BAD_REQUEST);

	const Parameters& parameters = content_disposition_it->second.getParameters();
	Parameters::const_iterator filename_it = parameters.find("filename");

	if (filename_it != parameters.end() && !filename_it->second.empty())
	{
		const std::string file_path = _location->upload_dir + "/" + filename_it->second;
		_upload_file(file_path);
		count += 1;
	}
}

void Response::_handle_post()
{
	if (!_location)
		throw (STATUS_NOT_FOUND);
	if (_location->upload_dir.empty())
		throw (STATUS_INTERNAL_ERR);

	const Headers& request_headers = _request.getHeaders();
	Headers::const_iterator request_content_type_it = request_headers.find(Headers::getTypeStr(HEADER_CONTENT_TYPE));

	if (request_content_type_it == request_headers.end())
		throw (STATUS_BAD_REQUEST);
	const std::string content_type = request_content_type_it->second.getValue();

	if (content_type != "multipart/form-data")
		throw (STATUS_UNSUPPORTED_MEDIA_TYPE);
	_handle_multipart_datas();
}

void Response::_upload_file(const std::string& file_path)
{
	if (_request.getContentLength() == 0)
		throw (STATUS_LENGTH_REQUIRED);

	std::ofstream file(file_path.c_str());

	if (!file.is_open())
		throw (STATUS_NOT_FOUND);
	file << _request.getMultipart()[0].getBody();
	file.close();
}

void Response::_handle_delete()
{
	switch (_resource_type)
	{
		case RT_FILE:
			_delete_file();
			break ;
		case RT_DIR:
		{
			const std::string& uri = _request.getStartLine().getUri().getPath();

			_delete_dir();
			break ;
		}
		default:
			throw (STATUS_NOT_IMPLEMENTED);
	}
}

void Response::_delete_dir()
{
	if (access(_resource_path.c_str(), W_OK) < 0)
		throw (STATUS_FORBIDDEN);
	const std::string command = "rm -rf " + _resource_path;
	if (std::system(command.c_str()) != 0)
		throw (STATUS_INTERNAL_ERR);
	start_line = StatusLine(STATUS_NO_CONTENT);
	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue("0")));
}

void Response::_delete_file()
{
	if (std::remove(_resource_path.c_str()) < 0)
		throw (STATUS_INTERNAL_ERR);
	start_line = StatusLine(STATUS_NO_CONTENT);
	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue("0")));

}

void Response::_handle_default_error(enum e_status_code status_code)
{
	// std::ifstream file(_default_error_page_path.c_str(), std::ios::binary);
	// const std::string status_tag = "[STATUS]";
	// std::string line;

	// if (!file.is_open())
	// 	throw (std::runtime_error("default error file error"));
	// while (getline(file, line))
	// {
	// 	while (true)
	// 	{
	// 		size_t pos = line.find(status_tag);
	// 		if (pos != std::string::npos)
	// 		{
	// 			size_t parameter_pos = pos + status_tag.length();
	// 			char parameter = line[parameter_pos];

	// 			switch (parameter)
	// 			{
	// 				case 'C':
	// 					line.replace(pos, status_tag.length() + 1, Utils::size_t_to_str(status_code));
	// 					break;
	// 				case 'M':
	// 					line.replace(pos, status_tag.length() + 1, StatusLine::_status_code_message[status_code]);
	// 					break;
	// 				default:
	// 					throw (std::runtime_error("default error file compromised"));
	// 			}
	// 		}
	// 		else
	// 			break;
	// 	}
	// 	content.append(line);
	// }
	std::stringstream ss;

	ss << "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>";
	ss << status_code;
	ss << "</title></head><body><h1>";
	ss << status_code;
	ss << "</h1><p>";
	ss << StatusLine::_status_code_message[status_code];
	ss << "</p></body></html>";
	start_line = StatusLine(status_code);
	content = ss.str();
	content_length = content.length();
}

void Response::_handle_error(enum e_status_code status_code)
{
	std::map<int, std::string>::const_iterator error_page_it = _conf.error_pages.find(status_code);

	if (error_page_it != _conf.error_pages.end())
	{
		std::string err_file_path = error_page_it->second;
		std::ifstream file(err_file_path.c_str(), std::ios::binary);
		std::stringstream file_content;
		std::string file_length_str;
		std::string file_extension;

		if (!file.is_open())
		{
			_handle_default_error(status_code);
			return ;
		}
		file_content << file.rdbuf();
		start_line = StatusLine(status_code);
		content = file_content.str();
		content_length = file_content.str().length();
	}
	else
	{
		_handle_default_error(status_code);
		headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue(Utils::size_t_to_str(content_length))));
		headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_TYPE), FieldValue(MimeType::get_mime_type("html"))));
	}
// 	switch (status_code)
// 	{
// 		// case STATUS_MOVED:
// 		// 	headers.insert(SingleField(Headers::getTypeStr(HEADER_LOCATION), FieldValue(_resource_path + "/")));
// 		// 	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue("0")));
// 		// 	headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_TYPE), FieldValue(MimeType::get_mime_type("html"))));
// 		// 	break ;
// 		// default:
// 			headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_LENGTH), FieldValue(Utils::size_t_to_str(content_length))));
// 			headers.insert(SingleField(Headers::getTypeStr(HEADER_CONTENT_TYPE), FieldValue(MimeType::get_mime_type("html"))));
// 	}
}

std::string Response::toHtml() const
{
	std::stringstream ss;

	ss << start_line.toHtml() << "\r\n";
	for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it)
		ss << it->first << ": " << it->second.getValue() << "\r\n";
	ss << "\r\n";
	if (content_length > 0)
		ss << content;
	return (ss.str());
}

std::string Response::toString() const
{
	std::stringstream ss;

	ss << start_line.toString() << "\\r\\n" << '\n';
	for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it)
		ss << it->first << ": " << it->second.getValue() << "\\r\\n" << '\n';
	ss << "\\r\\n" << '\n';
	// if (content_length > 0)
		// ss << content;
	return (ss.str());
}


const std::string& Response::getResourcePath() const
{
	return (_resource_path);
}

const Request& Response::getRequest() const
{
	return (_request);
}

void Response::content_append(const char *str, size_t n)
{
	content.append(str, n);
}

void Response::content_length_add(size_t n)
{
	content_length += n;
}

void Response::headers_insert(const SingleField& field)
{
	headers.insert(field);
}

size_t Response::getContentLength() const
{
	return (content_length);
}

void Response::setStatusLine(const StatusLine& status_line)
{
	start_line = status_line;
}
