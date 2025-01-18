#ifndef METHOD_HPP
#define METHOD_HPP

#include <string>
#include <vector>

enum e_method
{
	METHOD_GET = 0,
	METHOD_POST = 1,
	METHOD_DELETE = 2,
	NB_METHOD
};

class Method
{
public:
	Method();
	Method(const std::string& str);
	~Method();
	Method(const Method&);
	Method& operator=(const Method&);
	e_method getValue() const;
	std::string toString() const;
private:
	void _init();
	void _parse_method(const std::string& str);
	static std::vector<std::string> _methods;

	e_method _method;
};


#endif
