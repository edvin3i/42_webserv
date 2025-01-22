#ifndef URI_HPP
#define URI_HPP

#include <string>
#include <cctype>
#include <iostream>
#include "StatusCode.hpp"

class Uri
{
public:
	Uri();
	Uri(const std::string& str);
	~Uri();
	Uri(const Uri&);
	Uri& operator=(const Uri&);
	const std::string& getScheme() const;
	const std::string& getAuthority() const;
	const std::string& getPath() const;
	const std::string& getQuery() const;
	static void tester();

private:
	void _parse_uri(const std::string& str);
	void _parse_scheme(const std::string &str, size_t& pos);
	void _parse_authority(const std::string& str, size_t& pos);
	void _parse_path(const std::string& str, size_t& pos);
	void _parse_query(const std::string& str, size_t& pos);
	static void _test(const std::string& str, const std::string& s, const std::string& a, const std::string& p, const std::string& q, bool error);
	std::string _scheme;
	std::string _authority;
	std::string _path;
	std::string _query;
	static const size_t _max_uri_length;
};

#endif
