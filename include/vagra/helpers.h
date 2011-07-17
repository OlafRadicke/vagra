#ifndef VARGA_HELPERS_H
#define VARGA_HELPERS_H

#include <string>
#include <functional>

namespace vagra
{

bool emailValidate (const std::string&);
unsigned int str2unsigned(const std::string&);
unsigned int str2Pagenum(const std::string&);
bool isunderscore(char);
std::string space2underscore(const std::string&);
std::string underscore2space(const std::string& str);

struct invalidChar: public std::unary_function<const char&,bool>
{
	bool operator()(const char& c);
};

} //namespace vagra

#endif // VARGA_HELPERS_H
