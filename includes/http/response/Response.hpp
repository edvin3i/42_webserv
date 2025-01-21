#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <unistd.h>
# include <sys/wait.h>
#include <cstdlib>

#include "ErrorPages.hpp"
#include "../Message.hpp"
#include "../StatusLine.hpp"
#include "../Utils.hpp"
#include "../Request.hpp"
#include "../MimeType.hpp"
#include "../../logger/Logger.hpp"
#include "../../../includes/config/ServerConfig.hpp"
#include "../../Env.hpp"

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

class Response : protected Message<StatusLine>
{
public:
	Response(Logger & logger, const ServerConfig & conf, const LocationConfig  *location, const Request & request, Env env);
	~Response();
	std::string toHtml() const;
	std::string toString() const;
	const std::string& getResourcePath() const;
	const Request& getRequest() const;
	void content_append(const char *str, size_t n);
	void content_length_add(size_t n);
	void headers_insert(const SingleField&);
	size_t getContentLength() const;
	void setStatusLine(const StatusLine&);

private:
	Logger &_logger;
	const Request &_request;
	const ServerConfig &_conf;
	const LocationConfig* _location;
	std::string _str_content;
	std::string _resource_path;
	enum e_resource_type _resource_type;
	bool _has_index_file;
	std::string _index_file;
	static const size_t _cgi_buffer_size;
	Env _env;

	Response();
	Response(const Response & other);
	Response &operator=(const Response & other);
	void _check_location();
	void _check_resource();
	void _check_body_size(); // add body size check
	void _handle_get();
	void _handle_post();
	void _handle_delete();
	void _delete_file();
	void _delete_dir();
	void _handle_file(const std::string& filename);
	void _handle_dir();
	// bool _is_dir_has_index_file();
	bool _check_auto_index();
	void _handle_auto_index();
	void _upload_file(const std::string&);
	void _handle_error(enum e_status_code);
	void _handle_default_error(enum e_status_code status_code);
	void _check_method_allowed();
	void _handle_multipart_datas();
	void _handle_multipart_data(const BodyPart&, size_t&);
	void _handle_redirect();
	void _execute_cgi();
	bool _check_cgi_extension() const;
	bool _check_cgi_path() const;
	void _parse_cgi(const std::string&, std::string& cgi_content, Headers& cgi_headers);
	void _setEnvironmentVariables();
	void _readCgi(int fd, std::string&);
	void _sendCgi(int fd);
	void _buildCgiResponse(const std::string&);
	bool _check_redirect() const;
	void _check_index_file();

	static const std::string _html_auto_index;
	static const std::string _default_error_page_path;


};


#endif //RESPONSE_HPP
