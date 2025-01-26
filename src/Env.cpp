#include "../includes/Env.hpp"

Env::Env(char **env)
: std::map<std::string, std::string>()
{
	if (env == NULL)
		return ;
	for (size_t i = 0; env[i]; ++i)
	{
		std::string entry(env[i]);
		size_t pos = entry.find('=');

		if (pos != std::string::npos)
		{
			std::string key = entry.substr(0, pos);
			std::string value = entry.substr(pos + 1);

			insert(std::make_pair(key, value));
		}
	}
}

Env::Env(const Env& other)
: std::map<std::string, std::string>(other)
{
}

Env& Env::operator=(const Env& other)
{
	if (this != &other)
	{
		std::map<std::string, std::string>::operator=(other);
	}
	return (*this);
}

Env::~Env()
{
}

void Env::setEnv(const std::string& key, const std::string& value)
{
	insert(std::make_pair(key, value));
}


char **Env::toArray() const
{
	char **env = NULL;
	try
	{
		env = new char*[size() + 1];
		std::memset(env, 0, sizeof(env) * size() + 1);
		int index = 0;
		for (std::map<std::string, std::string>::const_iterator it = begin(); it != end(); ++it)
		{
			std::string str = it->first + "=" + it->second;

			env[index] = new char[str.length() + 1];
			std::strcpy(env[index], str.c_str());
			++index;
		}
		env[index] = NULL;
		return (env);
	}
	catch (const std::bad_alloc& e)
	{
		freeArray(env);
		throw;
	}
}

void Env::freeArray(char **env)
{
	if (env == NULL)
		return ;
	size_t i;
	for (i = 0; env[i]; ++i)
	{
		delete[] env[i];
		env[i] = NULL;
	}
	delete env[i];
	env[i] = NULL;
	delete[] env;
	env = NULL;
}
