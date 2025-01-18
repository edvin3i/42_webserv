#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <map>
#include <vector>
#include "FieldValue.hpp"
#include "Utils.hpp"
#include "Field.hpp"

class Headers : public std::multimap<std::string, FieldValue>
{
public:
	Headers();
	Headers(const std::vector<std::string>&);
	~Headers();
	Headers(const Headers&);
	Headers& operator=(const Headers&);
private:
	void _parse_fields(const std::vector<std::string>&);
	void _parse_field(const std::string&);
	void _parse_field_value(const std::string & str, const std::string & field_name);
	static bool _is_delimiter(char c);
	void _handle_quoted_str(const std::string& str, size_t& i, std::string& element);
	static const size_t _max_header_length;
};

#endif
