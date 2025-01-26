#ifndef BODY_HPP
#define BODY_HPP

#include <string>
#include "Headers.hpp"
#include "BodyPart.hpp"

class BodyPart;

class Body
{
public:
	Body();
	Body(const std::string& str);
	Body(const std::string& content, size_t content_length, const Headers& headers, bool is_chunked);
	~Body();
	Body(const Body&);
	Body& operator=(const Body&);
	const std::string& getContent() const;
	size_t getContentLength() const;
	bool is_mutlipart() const;
	const std::vector<BodyPart>& getMultipart() const;
	void setContent(const std::string& content);
	void setContentLength(size_t n);
	void addContent(const std::string& str);

private:
	void _checkContentLength(const std::string&);
	void _decode_chunked(const std::string& str);
	void _handle_multipart(const Headers& headers);
	static void _skip_newline(const std::string& str, size_t& i);

private:
	std::string _content;
	size_t _content_length;
	bool _is_multipart;
	std::vector<BodyPart> _multipart;
};

#endif
