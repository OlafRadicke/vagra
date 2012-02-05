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

#ifndef VARGA_BASEOBJECT_H
#define VARGA_BASEOBJECT_H

#include <string>
#include <vector>

#include <vagra/types.h>
#include <vagra/date.h>
#include <vagra/cachedcontext.h>

namespace vagra
{

class BaseObject
{
	CachedContext ctx;
	unsigned int oid; //Owner ID
	unsigned char read_level;//read only
	unsigned char add_level; //for creation of new objects with default permissions
	unsigned char write_level; //also allow update objects and modify permissions

	BaseObject() : ctx(0) {} //need tablename

	virtual void dbCommit(const unsigned int = 0) = NULL;
	virtual void clear() = NULL;

    protected:
	BaseObject(const std::string& _tablename) :
		ctx(0), oid(0),
		read_level(126),
		add_level(126),
		write_level(126),
		id(0), tablename(_tablename) {}

	/* call BaseObject("tablename", objId, authId), from derived initializer
	 * if authId is obmitted anonymous user 0 will be used
	 */
	BaseObject(const std::string&, const unsigned int, const unsigned int = 0);

	unsigned int id; //Object ID
	std::string tablename; //Database tablename

	vdate ctime;
	vdate mtime;

	/* call dbCommitBase(conn) in the same transaction as the
	 * derived dbCommit, thus there won't be any baseobject
	 * entries in the database if derived dbCommit throws
	 */
	void dbCommitBase(dbconn&, const unsigned int = 0);
	void clearBase();

    public:
	virtual operator bool() const;
	virtual BaseObject* operator->() = NULL;
	virtual ~BaseObject() {}

	const CachedContext& getContext() const;
	const unsigned int getId() const;
	const unsigned int getOwner() const;
	const unsigned char getReadLevel() const;
	const unsigned char getAddLevel() const;
	const unsigned char getWriteLevel() const;
	const std::string& getTable() const; //used by Search.setType()
	const unsigned char getAuthLevel(const unsigned int = 0) const;

	const vdate& getCTime() const;
	const vdate& getMTime() const;

	/* setContext always set all permissons to the Context defaults
	 * and also sets the owner to AuthId. Remember that dbCommit might
	 * become impossible if you set the owner != AuthId */
	void setContext(const CachedContext&, const unsigned int = 0);
	void setContext(const std::string&, const unsigned int = 0);
	void setContext(const unsigned int, const unsigned int = 0);

	void setOwner(const unsigned int, const unsigned int = 0);
	void setReadLevel(const unsigned char, const unsigned int = 0);
	void setAddLevel(const unsigned char, const unsigned int = 0);
	void setWriteLevel(const unsigned char, const unsigned int = 0);
};

} //namespace vagra

#endif // VAGRA_VARGA_BASEOBJECT_H
