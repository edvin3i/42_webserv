#include "../../../includes/http/response/Response.hpp"

const std::string Response::_html_auto_index = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Directory Index</title><style>body{font-family:'Arial',sans-serif;background-color:#f8f8f8;margin:0;padding:20px;}h1{color:#333;}ul{list-style-type:none;padding:0;}li{margin:10px 0;}a{color:#007bff;text-decoration:none;}a:hover{text-decoration:underline;}</style></head><body><h1>Index of The Directory</h1><ul>";

Response::Response(const Request& request, const LocationConfig* location)
: _request(request), _location(location)
{
	try
	{
		_check_location();
		_check_resource();
		//_handle_cgi();
		_check_method();
	}
	catch (enum e_status_code status_code)
	{
		_handle_error(status_code);
	}
}

Response::~Response()
{}

void Response::_check_location()
{
	if (!_location)
		throw (STATUS_NOT_FOUND);
	// else if (redirection)
	// {
	// 	redirection ?
	// }
	else
	{
		const std::vector<std::string>& allow_methods = _location->methods;
		const std::string& request_method = _request.start_line.getMethod();
		if (std::find(allow_methods.begin(), allow_methods.end(), request_method) == allow_methods.end())
			throw (STATUS_NOT_ALLOWED);
	}
}

void Response::_check_resource()
{
	struct stat file_stat;
	std::string resource_path;

	_resource = _location->root + _request.start_line.getUri();
	if (stat(_resource.c_str(), &file_stat) < 0)
		throw (STATUS_NOT_FOUND);
	else if (S_ISDIR(file_stat.st_mode))
		_resource_type = RT_DIR;
	else
		_resource_type = RT_FILE;
}

void Response::_check_method()
{
	const std::string method = _request.start_line.getMethod();

	if (method == "GET")
		_handle_get();
	else if (method == "POST")
		_handle_post();
	else
		_handle_delete();
}

void Response::_handle_get()
{
	switch (_resource_type)
	{
		case RT_FILE:
			_handle_file(_resource);
			break ;
		case RT_DIR:
			_handle_dir();
			break;
		default:
			throw (STATUS_INTERNAL_ERR);
	}
}

static std::string size_t_to_str(size_t n)
{
	std::stringstream ss;

	ss << n;
	return (ss.str());
}

void Response::_handle_file(const std::string& filename)
{
	std::ifstream file(filename.c_str(), std::ios::binary);
	std::stringstream file_content;
	std::string file_length_str;

	if (!file)
		throw (STATUS_INTERNAL_ERR);
	file_content << file.rdbuf();
	start_line = StatusLine(STATUS_OK);
	headers.insert(Field("Content-Length:", size_t_to_str(file_content.str().length())));
	headers.insert(Field("Content-Type:", "mime_type(not implemented)"));
	content = file_content.str();
	content_length = file_content.str().length();
}

void Response::_handle_dir()
{
	_check_dir();
	if (_is_dir_has_index_file())
		_handle_file(_resource + _location->index);
	else
		_check_auto_index();
}

void Response::_check_dir()
{
	const std::string& uri = _request.start_line.getUri();
	const std::string& method = _request.start_line.getMethod();

	if (uri[uri.size() - 1] != '/')
	{
		if (method == "DELETE")
			throw (STATUS_CONFLICT);
		else
			throw (STATUS_MOVED);
	}
}

bool Response::_is_dir_has_index_file()
{
	DIR *dir = opendir(_resource.c_str());
	struct dirent *file;
	bool index_found = false;

	if (!dir)
		return (false);
	while ((file = readdir(dir)))
	{
		if (strcmp(file->d_name, _location->index.c_str()) == 0)
		{
			closedir(dir);
			index_found = true;
			break ;
		}
	}
	closedir(dir);
	return (index_found);
}

void Response::_check_auto_index()
{
	if (_location->autoindex)
		_handle_auto_index();
	else
		throw (STATUS_FORBIDDEN);
}

void Response::_handle_auto_index()
{
	DIR *dir = opendir(_resource.c_str());
	struct dirent* file;

	if (!dir)
		throw (STATUS_INTERNAL_ERR);
	content.append(_html_auto_index);
	while (file = readdir(dir))
	{
		content.append("<li><a href=\"");
		content.append(file->d_name);
		content.append("\"");
		content.append(file->d_name);
		content.append("</a></li>");
	}
	content.append("</ul></body></html>");
	closedir(dir);

	start_line = StatusLine(STATUS_OK);
	headers.insert(Field("Content-Length", size_t_to_str(content.length())));
	headers.insert(Field("Content-Type", "text/html"));
	content_length = content.length();
}

void Response::_handle_post()
{
	if (_location->upload_dir.empty())
		throw (STATUS_FORBIDDEN);
	_upload_file();
	start_line = StatusLine(STATUS_CREATED);
}

std::string Response::get_filename()
{

	std::pair<Headers::iterator, Headers::iterator> content_disposition_it;
	const std::string filename_parameter = "filename=";
	std::string filename;

	if (headers.count("Content-Disposition") == 0)
		throw (std::runtime_error("Content-Disposition header does not exist"));
	content_disposition_it = headers.equal_range("Content-Disposition");
	if (content_disposition_it.first == content_disposition_it.second)
		throw (STATUS_BAD_REQUEST);
	for (Headers::iterator it = content_disposition_it.first; it != content_disposition_it.second; ++it)
	{
		std::string field_value = it->second;
		std::size_t filename_pos = field_value.find(filename_parameter);
		size_t i;
		if (filename_pos != std::string::npos)
		{
			if (field_value[filename_pos + filename_parameter.length()] != '\"')
				throw (STATUS_BAD_REQUEST);
			i = filename_pos + filename_parameter.length() + 1;
			while (true)
			{
				if (i == field_value.length())
					throw (STATUS_BAD_REQUEST);
				if (field_value[i] == '\"')
					return (filename);
				filename.push_back(field_value[i]);
				i += 1;
			}
		}
	}
	throw (STATUS_BAD_REQUEST);
}

void Response::_upload_file()
{
	if (_request.content_length == 0)
		throw (STATUS_INTERNAL_ERR);

	const std::string filename = get_filename();
	const std::string filepath = _location->upload_dir + "/" + filename;
	std::ofstream file(filename.c_str());

	if (!file.is_open())
		throw (STATUS_INTERNAL_ERR);
	file << content;
	file.close();
}

void Response::_handle_delete()
{
	throw (STATUS_NOT_IMPLEMENTED);
}

void Response::_handle_error(enum e_status_code status_code)
{
	throw (std::runtime_error("NO ERROR PAGES"));

	const std::string errFilePath = "www/error_404.html";
	std::ifstream file(errFilePath.c_str(), std::ios::binary);
	std::stringstream file_content;
	const std::string status_code_message;

	file_content << file.rdbuf();
	try
	{
		start_line = StatusLine(status_code);
	}
	catch (const std::exception& e)
	{
		throw (std::runtime_error("NOT IMPLEMENTED"));
	}
	content_length = file_content.str().length();
	headers.insert(Field("Content-Length", size_t_to_str(content_length)));
	headers.insert(Field("Content-Type", "text/html"));
	content = file_content.str();
	file.close();
}

std::string Response::toString() const
{
	std::stringstream ss;

	ss << start_line.toString() << "\r\n";
	for (Headers::const_iterator it = headers.begin(); it != headers.end(); ++it)
		ss << it->first << ": " << it->second << "\r\n";
	ss << "\r\n";
	if (content_length > 0)
		ss << content;
	return (ss.str());
}
