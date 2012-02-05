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

#ifndef VARGA_CONTEXT_H
#define VARGA_CONTEXT_H

#include <string>
#include <vector>

#include <vagra/types.h>

namespace vagra
{

class Context
{
	unsigned int id;
	std::string name;
	std::vector<unsigned int> admin; // allowed to modify Context

	std::vector<unsigned int> privileged;   // auth_level += 62
	std::vector<unsigned int> unprivileged; // auth_level += 14

	//Default privileges for objects with this context
	unsigned char read_level;
	unsigned char add_level;
	unsigned char write_level;

    public:
	Context() :
		id(0),
		read_level(126),
		add_level(126),
		write_level(126) {}

	Context(const std::string&, const unsigned int = 0);
	Context(const unsigned int, const unsigned int = 0);

	operator bool() const;
	operator int() const;
	operator unsigned int() const;
	Context* operator->() { return this; }

	const std::string& getName() const;
	const unsigned int getId() const;
	const unsigned char getReadLevel() const;
	const unsigned char getAddLevel() const;
	const unsigned char getWriteLevel() const;

	const unsigned char getAuthLevel(const unsigned int = 0) const;

	void setReadLevel(const unsigned char, const unsigned int = 0);
	void setAddLevel(const unsigned char, const unsigned int = 0);
	void setWriteLevel(const unsigned char, const unsigned int = 0);

	void addUnprivileged(const unsigned int, const unsigned int = 0);
	void addPrivileged(const unsigned int, const unsigned int = 0);
	void addAdmin(const unsigned int, const unsigned int = 0);
	void removeUnprivileged(const unsigned int, const unsigned int = 0);
	void removePrivileged(const unsigned int, const unsigned int = 0);
	void removeAdmin(const unsigned int, const unsigned int = 0);

	void dbCommit(const unsigned int = 0);
};

unsigned int getContextIdByName(const std::string&);
unsigned int getContextIdByName(const std::string& _name, dbconn&);

} //namespace vagra

#endif // VARGA_CONTEXT_H
