#include "../../includes/http/Uri.hpp"

Uri::Uri()
: _scheme(), _authority(), _path(), _query()
{}

Uri::Uri(const std::string& str)
{
	_parse_uri(str);
}

void Uri::_test(const std::string& str, const std::string& s, const std::string& a, const std::string& p, const std::string& q, bool exception)
{
	try
	{
		Uri uri(str);
		if (uri.getScheme() != s )
			throw (std::exception());
		if (uri.getAuthority() != a)
			throw (std::exception());
		if (uri.getPath() != p)
			throw (std::exception());
		if (uri.getQuery() != q)
			throw (std::exception());
		std::cerr << "Uri tester OK : " << str << "\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Uri tester error : " << str << "\n";
	}
	catch (enum e_status_code e)
	{
		if (exception)
			std::cerr << "Uri tester OK : " << str << " throw an excpetion\n";
		else
			std::cerr << "Uri tester error : " << str << "\n";
	}
}

void Uri::tester()
{
	_test("/where?q=now", "", "", "/where", "q=now", false);
	_test("http://www.example.org/pub/WWW/TheProject.html", "http", "www.example.org", "/pub/WWW/TheProject.html", "", false);
	_test("foo://example.com:8042/home", "", "", "", "", true);
	_test("http:/example.com/test", "", "", "", "", true);
}

void Uri::_parse_uri(const std::string& str)
{
	size_t pos = 0;

	_parse_scheme(str, pos);
	if (!_scheme.empty() && _scheme != "http")
		throw (STATUS_BAD_REQUEST);
	if (_path.empty())
		throw (STATUS_BAD_REQUEST);
}

void Uri::_parse_scheme(const std::string &str, size_t& pos)
{
	const size_t first_pos = pos;

	while (true)
	{
		switch (str.c_str()[pos])
		{
			case '\0':
				throw (STATUS_BAD_REQUEST);
			case '/':
				if (pos != first_pos)
					throw (STATUS_BAD_REQUEST);
				_parse_path(str, pos);
				return ;
			case ':':
				pos += 1;
				if (str.compare(pos, first_pos + 2, "//") == 0)
					pos += 2;
				else
					throw (STATUS_BAD_REQUEST);
				_parse_authority(str, pos);
				return ;
			case 'a'...'z': case 'A'...'Z':
				if (isupper(str[pos]))
					_scheme.push_back(tolower(str[pos]));
				else
					_scheme.push_back(str[pos]);
				pos += 1;
				break ;
			case '0'...'9': case '+': case '-': case '.':
				if (pos == first_pos)
					throw (STATUS_BAD_REQUEST);
				_scheme.push_back(str[pos]);
				pos += 1;
				break ;
			default:
				throw (STATUS_BAD_REQUEST);
		}
	}
}

void Uri::_parse_authority(const std::string& str, size_t &pos)
{
	while (true)
	{
		switch (str.c_str()[pos])
		{
			case '\0':
				throw (STATUS_BAD_REQUEST);
			case '/':
				_parse_path(str, pos);
				return ;
			default:
				_authority.push_back(str[pos]);
				pos += 1;
		}
	}
}

void Uri::_parse_path(const std::string& str, size_t &pos)
{
	while (true)
	{
		switch (str.c_str()[pos])
		{
			case '\0':
				return ;
			case '?':
				pos += 1;
				_parse_query(str, pos);
				return ;
			default:
				_path.push_back(str[pos]);
				pos += 1;
		}
	}
}

void Uri::_parse_query(const std::string& str, size_t &pos)
{
	_query = str.substr(pos);
}

Uri::~Uri()
{}

Uri::Uri(const Uri& other)
: _scheme(other._scheme), _authority(other._authority),
  _path(other._path), _query(other._query)
{}

Uri& Uri::operator=(const Uri& other)
{
	if (this != &other)
	{
		_scheme = other._scheme;
		_authority = other._authority;
		_path = other._path;
		_query = other._query;
	}
	return (*this);
}

const std::string& Uri::getScheme() const {return(_scheme);}
const std::string& Uri::getAuthority() const {return (_authority);}
const std::string& Uri::getPath() const {return (_path);}
const std::string& Uri::getQuery() const {return (_query);}

