#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <algorithm>
#include <vector>



class Utils
{
public:
	static const std::string whitespace;
	static bool is_whitespace(char c);
	static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
private:
	Utils();
	~Utils();
	Utils(const Utils&);
	Utils& operator=(const Utils&);
};

#endif
