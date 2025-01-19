#include "../../../includes/http/response/Response.hpp"

const std::string Response::_html_auto_index = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Directory Index</title><style>body{font-family:'Arial',sans-serif;background-color:#f8f8f8;margin:0;padding:20px;}h1{color:#333;}ul{list-style-type:none;padding:0;}li{margin:10px 0;}a{color:#007bff;text-decoration:none;}a:hover{text-decoration:underline;}</style></head><body><h1>Index of The Directory</h1><ul>";

const std::string Response::_default_error_page_path = "www/website/error_pages/default_error.html";

Response::Response(Logger & logger, const ServerConfig & conf, const LocationConfig  *location, const Request & request)
: _logger(logger), _request(request), _conf(conf), _location(location)
{
	if (_request.error())
	{
		_handle_error(_request.getErrorCode());
	}
	else
	{
		try
		{
			_check_method_allowed();
			switch (_request.start_line.getMethod().getValue())
			{
				case METHOD_GET:
					_check_resource();
					_handle_get();
					break ;
				case METHOD_POST:
					_handle_post();
					break;
				case METHOD_DELETE:
					_check_resource();
					_handle_delete();
					break ;
			}
			// _check_resource();
			// //_handle_cgi();
			// _check_method();
		}
		catch (enum e_status_code status_code)
		{
			_handle_error(status_code);
		}
	}
}


Response::~Response()
{}

void Response::_check_method_allowed()
{
	if (_location)
	{
		const std::vector<std::string>& allow_methods = _location->methods;
		const std::string& request_method = _request.start_line.getMethod().toString();
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
	const std::string& uri_path = _request.start_line.getUri().getPath();

//  antonin
	// _resource_path = _conf.root + _request.start_line.getUri().getPath();
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
	}
	else {
		std::ostringstream ss; ss << "Resource is a file" << std::endl;
		_logger.writeToLog(DEBUG, ss.str());
		ss.str("");
		_resource_type = RT_FILE;
	}
}

// void Response::_check_method()
// {
// 	const std::string method = _request.start_line.getMethod();

// 	if (method == "GET")
// 		_handle_get();
// 	else if (method == "POST")
// 		_handle_post();
// 	else
// 		_handle_delete();
// }

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

void Response::_handle_file(const std::string& filename)
{
	std::ifstream file(filename.c_str(), std::ios::binary);
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
	headers.insert(SingleField("Content-Length", FieldValue(Utils::size_t_to_str(file_content.str().length()))));
	headers.insert(SingleField("Content-Type", FieldValue(_filename_to_mime_type(filename))));

  // 	headers.insert(Field("Content-Length", FieldValue(size_t_to_str(file_size))));
  // 	headers.insert(Field("Content-Type", FieldValue(_filename_to_mime_type(filename))));
	content = file_content.str();
	content_length = file_size;
}

void Response::_handle_dir()
{
	const std::string& uri = _request.start_line.getUri().getPath();
	if (uri[uri.length() - 1] != '/') {
		start_line = StatusLine(STATUS_MOVED);
		headers.insert(SingleField("Content-Length", FieldValue("0")));
		headers.insert(SingleField("Content-Type", FieldValue(MimeType::get_mime_type("html"))));
		headers.insert(SingleField("Location", FieldValue(uri + "/")));
		return;
	}
	if (_is_dir_has_index_file())
		_handle_file(_resource_path + _conf.index);
	else
		_check_auto_index();
}

bool Response::_is_dir_has_index_file()
{
	DIR *dir = opendir(_resource_path.c_str());
	struct dirent *file;
	bool index_found = false;
	std::string index_file;

	if (_location && !_location->index.empty())
		index_file = _location->index;
	else
		index_file = _conf.index;

	if (!dir)
		return (false);
	while ((file = readdir(dir)))
	{
		if (strcmp(file->d_name, index_file.c_str()) == 0)
		{
			index_found = true;
			break ;
		}
	}
	closedir(dir);
	return (index_found);
}

void Response::_check_auto_index()
{
	if (_location && _location->autoindex)
		_handle_auto_index();
	else
		throw (STATUS_FORBIDDEN);
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
			content.append(_request.start_line.getUri().getPath());
			const std::string& path = _request.start_line.getUri().getPath();
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
	headers.insert(SingleField("Content-Length", FieldValue(Utils::size_t_to_str(content.length()))));
	headers.insert(SingleField("Content-Type", FieldValue(MimeType::get_mime_type("html"))));
	content_length = content.length();
}

void Response::_check_body_size() {
	// if request has body
	if (_request.content_length > 0) {
		// compare with server_config limit
		if (_conf.client_max_body_size > 0 && _request.content_length > _conf.client_max_body_size) {
			_logger.writeToLog(ERROR, "Request body size exceeds client_max_body_size limit");
			throw (STATUS_TOO_LARGE);
		}
	}
}

void Response::_handle_post()
{
	if (!_location)
		throw (STATUS_NOT_FOUND);
	if (_location->upload_dir.empty())
		throw (STATUS_INTERNAL_ERR);

	Headers::const_iterator request_content_type_it = _request.headers.find("Content-Type");

	if (request_content_type_it == _request.headers.end())
		throw (STATUS_BAD_REQUEST);
	const std::string content_type = request_content_type_it->second.getValue();

	if (content_type != "multipart/form-data")
		throw (STATUS_UNSUPPORTED_MEDIA_TYPE);
	const std::string filename = get_filename();
	std::string file_path;
	_upload_file(_location->upload_dir + "/" + filename);
	start_line = StatusLine(STATUS_CREATED);
	headers.insert(SingleField("Content-Length", FieldValue("0")));
}

std::string Response::get_filename()
{
	const std::vector<BodyPart>& multipart = _request.getMultipart();

	if (multipart.empty())
		throw (STATUS_BAD_REQUEST);
	const Headers& body_headers = multipart[0].getHeaders();
	std::pair<Headers::const_iterator, Headers::const_iterator> content_disposition_it;
	std::string filename;

	if (body_headers.count("Content-Disposition") == 0)
		throw (STATUS_BAD_REQUEST);
	content_disposition_it = body_headers.equal_range("Content-Disposition");
	for (Headers::const_iterator it = content_disposition_it.first; it != content_disposition_it.second; ++it)
	{
		const Parameters& parameters = it->second.getParameters();
		Parameters::const_iterator filename_it = parameters.find("filename");

		if (filename_it == parameters.end() || filename_it->second.empty())
			throw (STATUS_BAD_REQUEST);
		filename = filename_it->second;
	}
	return (filename);
}

void Response::_upload_file(const std::string& file_path)
{
	if (_request.content_length == 0)
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
			const std::string& uri = _request.start_line.getUri().getPath();

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
	headers.insert(SingleField("Content-Length", FieldValue("0")));
}

void Response::_delete_file()
{
	if (std::remove(_resource_path.c_str()) < 0)
		throw (STATUS_INTERNAL_ERR);
	start_line = StatusLine(STATUS_NO_CONTENT);
	headers.insert(SingleField("Content-Length", FieldValue("0")));

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
	}
	switch (status_code)
	{
		case STATUS_MOVED:
			headers.insert(SingleField("Location", FieldValue(_resource_path + "/")));
			headers.insert(SingleField("Content-Length", FieldValue("0")));
			headers.insert(SingleField("Content-Type", FieldValue(MimeType::get_mime_type("html"))));
			break ;
		default:
			headers.insert(SingleField("Content-Length", FieldValue(Utils::size_t_to_str(content_length))));
			headers.insert(SingleField("Content-Type", FieldValue(MimeType::get_mime_type("html"))));
	}
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
