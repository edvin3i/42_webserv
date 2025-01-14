#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

const std::string whitespace = " \t";

class Utils
{
public:
	static bool is_whitespace(char c);
private:
	Utils();
	~Utils();
	Utils(const Utils&);
	Utils& operator=(const Utils&);
};

#endif
