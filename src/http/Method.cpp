#include "../../includes/http/Method.hpp"

std::vector<std::string> Method::_methods;

void Method::_init()
{
	_methods.resize(NB_METHOD);
	_methods[METHOD_GET] = "GET";
	_methods[METHOD_POST] = "POST";
	_methods[METHOD_DELETE] = "DELETE";
}

Method::Method()
: _method()
{
	_init();
}

Method::Method(const std::string& str)
{
	_init();
	_parse_method(str);
}

Method::~Method()
{}

Method::Method(const Method& other)
: _method(other._method)
{
	_init();
}
Method& Method::operator=(const Method& other)
{
	if (this != &other)
	{
		_method = other._method;
	}
	return (*this);
}
e_method Method::getValue() const
{
	return (_method);
}

std::string Method::toString() const
{
	return (_methods[_method]);
}

void Method::_parse_method(const std::string& str)
{
	bool is_method_exist = false;
	size_t i;
	for (i = 0; i < _methods.size(); ++i)
	{
		if (str == _methods[i])
		{
			is_method_exist = true;
			break ;
		}
	}
	if (is_method_exist == false)
		throw (std::exception());
	_method = static_cast<e_method>(i);
}
