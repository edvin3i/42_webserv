#ifndef FIELD_VALUE_HPP
#define FIELD_VALUE_HPP

#include <string>
#include <map>
#include "StatusCode.hpp"
#include "Utils.hpp"

typedef std::map<std::string, std::string> Parameters;
typedef std::pair<std::string, std::string> Parameter;

class FieldValue
{
public:
	FieldValue();
	FieldValue(const std::string& value);
	~FieldValue();
	FieldValue(const FieldValue&);
	FieldValue& operator=(const FieldValue&);

	const std::string& getValue() const;
	const Parameters& getParameters() const;
	void setValue(const std::string &value);
	void addParameters(const std::string& param_name, const std::string& param_value);
	void _parse_value(const std::string&);
	void _parse_parameters(const std::string&);
	void _handle_quote(const std::string&, std::string& text, size_t& i);
private:
	std::string _value;
	Parameters _parameters;
	enum parse_state {STATE_PARAM_NAME, STATE_PARAM_VALUE, NB_STATE};
};

#endif
