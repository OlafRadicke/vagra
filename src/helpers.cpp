#include <sstream>
#include <algorithm>
#include <cxxtools/regex.h>
#include <vagra/helpers.h>

namespace vagra
{

bool emailValidate (const std::string& mail)
{
	cxxtools::Regex checkEmail("[aA-zZ]+@[aA-zZ][aA-zZ]+\\.[aA-zZ][aA-zZ]+$");
	return checkEmail.match(mail);
}

unsigned int str2unsigned(const std::string& s)
{
	unsigned int rval = 0;
	if(s.empty())
		return rval;
	std::istringstream iss(s);
	iss >> rval;
	return rval;
}

unsigned int str2Pagenum(const std::string& s)
{
	unsigned int page = 1;
	if(s.empty())
		return page;
	std::istringstream iss(s);
	iss >> page;
	if(page == 0)
		page = 1;
	return page;
}

bool isunderscore(char c)
{
	return c == '_';
}

std::string space2underscore(const std::string& str)
{
	std::string rstr(str);

	std::replace_if(rstr.begin(), rstr.end(), isspace, '_');

	return rstr;
}

std::string underscore2space(const std::string& str)
{
	std::string rstr(str);

	std::replace_if(rstr.begin(), rstr.end(), isunderscore, ' ');

	return rstr;
}

bool invalidChar::operator()(const char& c)
{
	std::locale loc;
	if(std::isalnum(c))
		return false;
	if(c == '_' || c == '+' || c == '-' )
		return false;
	return true;
}

} //namespace vagra
