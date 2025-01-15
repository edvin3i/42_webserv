#include "../../../includes/http/response/Response.hpp"

const std::string Response::_html_auto_index = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Directory Index</title><style>body{font-family:'Arial',sans-serif;background-color:#f8f8f8;margin:0;padding:20px;}h1{color:#333;}ul{list-style-type:none;padding:0;}li{margin:10px 0;}a{color:#007bff;text-decoration:none;}a:hover{text-decoration:underline;}</style></head><body><h1>Index of The Directory</h1><ul>";

const std::string Response::_default_error_page_path = "www/website/error_pages/default_error.html";

Response::Response(const Request& request, const ServerConfig& conf, const LocationConfig* location)
: _request(request), _conf(conf), _location(location)
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
	// else if (redirection)
	// {
	// 	redirection ?
	// }
	if (_location)
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

	_resource_path = _conf.root + _request.start_line.getUri();
	if (stat(_resource_path.c_str(), &file_stat) < 0)
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
			_handle_file(_resource_path);
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
		headers.insert(Field("Location", FieldValue(_resource_path + "/")));
		headers.insert(Field("Content-Length", FieldValue("0")));
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
	if (_location->autoindex)
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
	if (_location->upload_dir.empty())
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
	const std::string filepath = _location->upload_dir + "/" + filename;
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
	if (access(_resource_path.c_str(), W_OK) != 0)
		throw (STATUS_FORBIDDEN);
	const std::string command = "rm -rf " + _resource_path;
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
	if (std::remove(_resource_path.c_str()) != 0)
		throw (STATUS_INTERNAL_ERR);
	start_line = StatusLine(STATUS_NO_CONTENT);
	headers.insert(Field("Content-Length", FieldValue("0")));

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
	// 					line.replace(pos, status_tag.length() + 1, size_t_to_str(status_code));
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
	start_line = StatusLine(STATUS_NOT_FOUND);
	content = ss.str();
	content_length = content.length();
	headers.insert(Field("Content-Length", FieldValue(size_t_to_str(content_length))));
	headers.insert(Field("Content-Type", FieldValue(MimeType::get_mime_type("html"))));
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

		if (!file)
			_handle_default_error(status_code);
		file_content << file.rdbuf();
		start_line = StatusLine(status_code);
		headers.insert(Field("Content-Length:", FieldValue(size_t_to_str(file_content.str().length()))));
		headers.insert(Field("Content-Type:", FieldValue(_filename_to_mime_type(err_file_path))));
		content = file_content.str();
		content_length = file_content.str().length();
	}
	else
		_handle_default_error(status_code);
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
	if (content_length > 0)
		ss << content;
	return (ss.str());
}
