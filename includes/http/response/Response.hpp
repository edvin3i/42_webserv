#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <unistd.h>

#include "ErrorPages.hpp"
#include "../Message.hpp"
#include "../StatusLine.hpp"
#include "../Utils.hpp"
#include "../Request.hpp"
#include "../MimeType.hpp"
#include "../../logger/Logger.hpp"
#include "../../../includes/config/ServerConfig.hpp"

/*
	Response class is a base class for all response classes (CGIResponse, StaticResponse, etc.)
	It contains all the common methods and fields for all response classes
	Specific response class chooses at the ResponseBuilder class (factory)
*/


enum e_resource_type
{
	RT_FILE,
	RT_DIR
};

class Response : public Message<StatusLine>
{
public:
	Response(Logger & logger, const ServerConfig & conf, const LocationConfig  *location, const Request & request);
	Response();
	//Response(const Request&, const ServerConfig&, const LocationConfig*);
	~Response();
	Response(const Response & other);
	Response &operator=(const Response & other);
	// void _buildStatusLine(int code);
	// void _addHeader(std::string key, std::string val);
	std::string toHtml() const;
	std::string toString() const;

private:
	Logger &_logger;
	const Request &_request;
	const ServerConfig &_conf;
	const LocationConfig* _location;
	std::string _str_content;
	std::string _resource_path;
	enum e_resource_type _resource_type;
	void _check_location();
	void _check_resource();
	void _check_method();
	void _check_body_size(); // add body size check
	void _handle_get();
	void _handle_post();
	void _handle_delete();
	void _delete_file();
	void _delete_dir();
	void _handle_file(const std::string& filename);
	void _handle_dir();
	void _check_dir();
	bool _is_dir_has_index_file();
	void _check_auto_index();
	void _handle_auto_index();
	void _upload_file(const std::string&);
	std::string get_filename();
	void _handle_error(enum e_status_code);
	void _handle_default_error(enum e_status_code status_code);
	void _check_method_allowed();

	static const std::string _html_auto_index;
	static const std::string _default_error_page_path;

	
};


#endif //RESPONSE_HPP
