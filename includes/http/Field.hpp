#ifndef FIELD_HPP
#define FIELD_HPP

#include "FieldValue.hpp"
#include <utility>

class Field : public std::pair<std::string, FieldValue>
{
public:
	Field();
	Field(const std::string&, const FieldValue&);
	~Field();
	Field(const Field& other);
	Field& operator=(const Field& other);
};

#endif
