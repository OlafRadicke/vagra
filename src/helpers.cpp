/*
 * Copyright (C) 2011 by Julian Wiesener
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

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
