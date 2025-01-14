#include "../../../includes/http/response/Response.hpp"

const std::string Response::_html_auto_index = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Directory Index</title><style>body{font-family:'Arial',sans-serif;background-color:#f8f8f8;margin:0;padding:20px;}h1{color:#333;}ul{list-style-type:none;padding:0;}li{margin:10px 0;}a{color:#007bff;text-decoration:none;}a:hover{text-decoration:underline;}</style></head><body><h1>Index of The Directory</h1><ul>";

Response::Response(const Request& request, const ServerConfig& conf)
: _request(request), _conf(conf)
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
	// if (!_location)
	// 	throw (STATUS_NOT_FOUND);
	// else if (redirection)
	// {
	// 	redirection ?
	// }
	// else
	// {
	// 	const std::vector<std::string>& allow_methods = _location->methods;
	// 	const std::string& request_method = _request.start_line.getMethod();
	// 	if (std::find(allow_methods.begin(), allow_methods.end(), request_method) == allow_methods.end())
	// 		throw (STATUS_NOT_ALLOWED);
	// }
}

void Response::_check_resource()
{
	struct stat file_stat;
	std::string resource_path;

	_resource = _conf.root + _request.start_line.getUri();
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
	std::transform(extension.begin(), extension.end(), lower_extension.begin(), my_tolower);
	return (MimeType::get_mime_type(lower_extension));
}

void Response::_handle_file(const std::string& filename)
{
	std::ifstream file(filename.c_str(), std::ios::binary);
	std::stringstream file_content;
	std::string file_length_str;
	std::string file_extension;

	if (!file)
		throw (STATUS_INTERNAL_ERR);
	file_content << file.rdbuf();
	start_line = StatusLine(STATUS_OK);
	headers.insert(Field("Content-Length:", FieldValue(size_t_to_str(file_content.str().length()))));
	headers.insert(Field("Content-Type:", FieldValue(_filename_to_mime_type(filename))));
	content = file_content.str();
	content_length = file_content.str().length();
}

void Response::_handle_dir()
{
	const std::string& uri = _request.start_line.getUri();
	if (uri[uri.length() - 1] != '/')
	{
		start_line = StatusLine(STATUS_MOVED);
		headers.insert(Field("Location", FieldValue(_resource + "/")));
		headers.insert(Field("Content-Length", FieldValue("0")));
	}
	if (_is_dir_has_index_file())
		_handle_file(_resource + _conf.index);
	else
		_check_auto_index();
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
		if (strcmp(file->d_name, _conf.index.c_str()) == 0)
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
	// if (_conf.autoindex)
	// 	_handle_auto_index();
	// else
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
	headers.insert(Field("Content-Length", FieldValue(size_t_to_str(content.length()))));
	headers.insert(Field("Content-Type", FieldValue(MimeType::get_mime_type("html"))));
	content_length = content.length();
}

void Response::_handle_post()
{
	// if (_conf.upload_dir.empty())
		throw (STATUS_FORBIDDEN);
	_upload_file();
	start_line = StatusLine(STATUS_CREATED);
	headers.insert(Field("Content-Length", FieldValue("0")));
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
		std::string field_value = it->second.getValue();
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
	throw (STATUS_NOT_IMPLEMENTED);
	if (_request.content_length == 0)
		throw (STATUS_INTERNAL_ERR);

	const std::string filename = get_filename();
	// const std::string filepath = _location->upload_dir + "/" + filename;
	std::ofstream file(filename.c_str());

	if (!file.is_open())
		throw (STATUS_INTERNAL_ERR);
	file << content;
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
			const std::string& uri = _request.start_line.getUri();

			if (uri[uri.length() - 1] != '/')
				start_line = StatusLine(STATUS_CONFLICT);
			break ;
		}
		default:
			throw (STATUS_NOT_IMPLEMENTED);
	}
}

void Response::_delete_dir()
{
	if (access(_resource.c_str(), W_OK) != 0)
		throw (STATUS_FORBIDDEN);
	const std::string command = "rm -rf " + _resource;
	if (std::system(command.c_str()) == 0)
	{
		start_line = StatusLine(STATUS_NO_CONTENT);
		headers.insert(Field("Content-Length", FieldValue("0")));
	}
	else
		throw (STATUS_INTERNAL_ERR);
}

void Response::_delete_file()
{
	if (std::remove(_resource.c_str()) != 0)
		throw (STATUS_INTERNAL_ERR);
	start_line = StatusLine(STATUS_NO_CONTENT);
	headers.insert(Field("Content-Length", FieldValue("0")));

}

void Response::_handle_error(enum e_status_code status_code)
{
	throw (std::runtime_error("NO ERROR PAGES"));

	const std::string errFilePath = "?";
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
	headers.insert(Field("Content-Length", FieldValue(size_t_to_str(content_length))));
	headers.insert(Field("Content-Type", FieldValue(MimeType::get_mime_type("html"))));
	content = file_content.str();
	file.close();
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
