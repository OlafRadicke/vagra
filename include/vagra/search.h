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

#ifndef VARGA_SEARCH_H
#define VARGA_SEARCH_H

#include <string>
#include <vector>

#include <vagra/baseobject.h>
#include <vagra/cachedcontext.h>

namespace vagra
{

class Search
{
    protected:
	std::string search_string;
	std::string search_key;
	std::vector<unsigned int> results;
	
	std::string table;
	std::string order_col;
	bool order_asc;
	unsigned int limit;
	unsigned int read_level;
	unsigned int cid;
	unsigned int oid;

	virtual void dbSearch();
	virtual void genSearchString();

	//utilized by CachedSearch template
	virtual void genSearchKey();
    public:
	Search() :
		order_col("id"),
		order_asc(true),
		limit(0),
		read_level(0),
		cid(0), oid(0) {}
	virtual ~Search() {}

	/* use either setType(obj()) with noncached objects derivated from
	 * BaseObject to set the table to search on, OR use setTable("table") */
	void setType(const BaseObject&);
	void setTable(const std::string&);

	void setOrderColumn(const std::string&);
	void setOrderAsc(const bool);
	void setOwner(const unsigned int);
	void setContext(const unsigned int);
	void setContext(const std::string&);
	void setContext(const CachedContext&);
	void setReadLevel(const unsigned int);
	void setLimit(const unsigned int);

	const std::vector<unsigned int>& getResults();
};

} //namespace vagra

#endif // VARGA_SEARCH_H
