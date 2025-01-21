#ifndef ENV_HPP
#define ENV_HPP

#include <string>
#include <map>
#include <cstring>

class Env : protected std::map<std::string, std::string>
{
public:
	Env(char **env);
	Env(const Env&);
	Env& operator=(const Env&);
	~Env();

	void setEnv(const std::string &key, const std::string &value);
	char **toArray() const;
	static void freeArray(char **env);
private:
	Env();
};

#endif
