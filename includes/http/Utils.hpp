#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <algorithm>
#include <vector>
#include <sstream>

class Utils
{
public:
	static const std::string whitespace;
	static bool is_whitespace(char c);
	static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
	static std::string size_t_to_str(size_t n);
	static std::string str_trim(const std::string &str);
private:
	Utils();
	~Utils();
	Utils(const Utils&);
	Utils& operator=(const Utils&);
};

#endif
