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

#ifndef VARGA_BAASEOBJECT_H
#define VARGA_BAASEOBJECT_H

#include <string>
#include <vector>

#include <vagra/types.h>
#include <vagra/date.h>

namespace vagra
{

class BaseObject
{
	BaseObject() {} //need tablename

	virtual void dbCommit() = NULL;
	virtual void clear() = NULL;
	std::string tablename;

    protected:

	BaseObject(const std::string& _tablename) :
		tablename(_tablename),
		id(0), oid(0), gid(0),
		read_level(126),
		write_level(126) {}

	BaseObject(const std::string&, const unsigned int);

	unsigned int id;
	unsigned int oid; //Owner ID
	unsigned int gid; //Group ID
	unsigned char read_level;
	unsigned char write_level;
	vdate ctime;
	vdate mtime;

	/* call dbCommitBase(conn) in the same transaction as the
	 * derived dbCommit, thus there won't be any baseobject
	 * entries in the database if derived dbCommit throws
	 */
	void dbCommitBase(dbconn&);
	void clearBase();

    public:
	virtual operator bool() const;
	virtual BaseObject* operator->() = NULL;
	virtual ~BaseObject() {}

	const unsigned int getId() const;
	const unsigned int getOwner() const;
	const unsigned int getGroup() const;
	const unsigned char getReadLevel() const;
	const unsigned char getWriteLevel() const;

	const vdate& getCTime() const;
	const vdate& getMTime() const;


	void setOwner(unsigned int);
	void setGroup(unsigned int);
	void setReadLevel(unsigned char);
	void setWriteLevel(unsigned char);
};

} //namespace vagra

#endif // VAGRA_VARGA_BAASEOBJECT_H
