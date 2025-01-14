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
	// Response(Logger & logger, const ServerConfig & srv_conf, const Request & request);
	Response(const Request&, const ServerConfig&);
	~Response();

	// virtual std::string build() = 0;
	const std::string& str() const;

// protected:
private:
	// Logger &_logger;
	const Request &_request;
	const ServerConfig &_conf;

public:
	Response();
	Response(const Response & other);
	Response &operator=(const Response & other);
	void _buildStatusLine(int code);
	void _addHeader(std::string key, std::string val);
	std::string toHtml() const;

private:
	std::string _str_content;
	std::string _resource;
	enum e_resource_type _resource_type;
	void _check_location();
	void _check_resource();
	void _check_method();
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
	void _upload_file();
	std::string get_filename();
	void _handle_error(enum e_status_code);

	static const std::string _html_auto_index;
};


#endif //RESPONSE_HPP
