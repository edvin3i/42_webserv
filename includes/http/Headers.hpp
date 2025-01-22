#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <map>
#include <vector>
#include "FieldValue.hpp"
#include "Utils.hpp"
#include "Field.hpp"

typedef enum e_header_type
{
	HEADER_CONTENT_TYPE,
	HEADER_CONTENT_LENGTH,
	HEADER_LOCATION,
	HEADER_HOST,
	HEADER_CONTENT_DISPOSITION,
	HEADER_TRANSFER_ENCODING,
	NB_HEADER_TYPE
}	HeaderType;

typedef std::pair<std::string, FieldValue> SingleField;

class Headers : public std::multimap<std::string, FieldValue>
{
public:
	Headers();
	Headers(const std::vector<std::string>&);
	~Headers();
	Headers(const Headers&);
	Headers& operator=(const Headers&);

	static std::string getTypeStr(HeaderType);
private:
	static void _init();
	void _parse_fields(const std::vector<std::string>&);

	static bool is_init;
	static std::vector<std::string> _type_to_str;
};

#endif
