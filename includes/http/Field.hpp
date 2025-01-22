#ifndef FieldListLIST_HPP
#define FieldListLIST_HPP

#include "FieldValue.hpp"
#include <utility>

class Field : public std::pair<std::string, std::vector< FieldValue> >
{
public:
	Field();
	Field(const std::string& str);
	Field(const std::string& name, const std::vector<FieldValue>& values);
	~Field();
	Field(const Field& other);
	Field& operator=(const Field& other);
	size_t count_values() const;
private:
	void _parse_field(const std::string& str);
	void _parse_field_list(const std::string& str);
	static const size_t _max_field_length;
};

#endif
