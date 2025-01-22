#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <cctype>

class Utils
{
public:
	static const std::string whitespace;
	static bool is_whitespace(char c);
	static std::vector<std::string> split_path(const std::string& str);
	static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
	static std::string size_t_to_str(size_t n);
	static int stoi(const std::string&);
	static unsigned long stoul(const std::string& str);
	static std::string str_trim(const std::string &str);
	static std::string to_lowercase(const std::string& str);
	template <class InputIt, class UnaryPred> static bool all_of(InputIt first, InputIt last, UnaryPred p);
	template <class InputIt, class UnaryPred> static InputIt find_if_not(InputIt first, InputIt last, UnaryPred q);
	template <class T> static std::string NumberToString(T Number);
private:
	Utils();
	~Utils();
	Utils(const Utils&);
	Utils& operator=(const Utils&);
};

template <class InputIt, class UnaryPred>
InputIt Utils::find_if_not(InputIt first, InputIt last, UnaryPred q)
{
	for (; first != last; ++first)
		if (!q(*first))
			return (first);
	return (last);
}

template <class InputIt, class UnaryPred>
bool Utils::all_of(InputIt first, InputIt last, UnaryPred p)
{
	return (Utils::find_if_not(first, last, p) == last);
}

template <class T>
std::string Utils::NumberToString(T n)
{
	std::ostringstream ss;
	ss << n;
	return (ss.str());
}

#endif
