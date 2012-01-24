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

#ifndef VARGA_USER_H
#define VARGA_USER_H

#include <string>

#include <vagra/types.h>
#include <vagra/baseobject.h>

namespace vagra
{

class User: public BaseObject
{
	std::string logname;
	std::string dispname;
	std::string surname;
	std::string name;
	std::string mail;

    public:
	User() : BaseObject("vuser") {}
	explicit User(const unsigned int, const unsigned int = 0);
	operator bool() const;
	operator int() const;
	operator unsigned int() const;
	User* operator->() { return this; }
	void clear();

	unsigned int getId() const;
	const std::string& getLogname() const;
	const std::string& getDispname() const;
	const std::string& getSurname() const;
	const std::string& getName() const;
	const std::string& getMail() const;

	void setLogname(const std::string&);
	void setDispname(const std::string&);
	void setSurname(const std::string&);
	void setName(const std::string&);
	void setMail(const std::string&);

	void dbCommit(const unsigned int = 0);
};

unsigned int getUidByLogname(const std::string&);
unsigned int getUidByLogname(const std::string&, dbconn&);

unsigned int getUidByDispname(const std::string&);
unsigned int getUidByDispname(const std::string&, dbconn&);

} //namespace vagra

#endif // VARGA_USER_H
