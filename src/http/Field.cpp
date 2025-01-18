#include "../../includes/http/Field.hpp"

Field::Field()
: std::pair<std::string, FieldValue>()
{}

Field::Field(const std::string& name, const FieldValue& value)
: std::pair<std::string, FieldValue>(name, value)
{}

Field::~Field() {}

Field::Field(const Field& other)
: std::pair<std::string, FieldValue>(other)
{}

Field& Field::operator=(const Field& other)
{
	if (this != &other)
	{
		std::pair<std::string, FieldValue>::operator=(other);
	}
	return (*this);
}
