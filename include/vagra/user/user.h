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

#include <vagra/passwd/passwd.h>

namespace vagra
{

class User: public BaseObject
{
	std::string logname;
	std::string dispname;

	Passwd password;

	void Init(const BaseAuth& = BaseAuth());

    public:
	User() : BaseObject("vuser") {}
	explicit User(const unsigned int, const BaseAuth& = BaseAuth());
	explicit User(const std::string&, const BaseAuth& = BaseAuth());

	operator bool() const;
	operator int() const;
	operator unsigned int() const;
	User* operator->() { return this; }
	void clear();

	const std::string& getLogname() const;
	const std::string& getDispname() const;
	const Passwd& getPasswd() const;

	void setLogname(const std::string&);
	void setDispname(const std::string&);
	void setPasswd(const Passwd&);
	void setPasswd(const std::string&);

	std::string setRandomPasswd(const BaseAuth& = BaseAuth());

	void dbCommit(const BaseAuth& = BaseAuth());
	void passwdCommit(const BaseAuth& = BaseAuth());

	static unsigned int getIdByName(const std::string&);
};

unsigned int getUidByLogname(const std::string&);
unsigned int getUidByLogname(const std::string&, dbconn&);

unsigned int getUidByDispname(const std::string&);
unsigned int getUidByDispname(const std::string&, dbconn&);

} //namespace vagra

#endif // VARGA_USER_H
