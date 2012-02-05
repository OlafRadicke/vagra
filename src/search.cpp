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

#include <libintl.h>
#include <stdexcept>
#include <algorithm>
#include <sstream>

#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <tntdb/result.h>
#include <tntdb/row.h>

#include <vagra/search.h>
#include <vagra/helpers.h>
#include <vagra/nexus.h>

namespace vagra
{

log_define("vagra")

//begin Search

void Search::setType(const BaseObject& _obj)
{
	setTable(_obj.getTable());
}

void Search::setTable(const std::string& _table)
{
	if(_table.empty() || std::find_if(_table.begin(), _table.end(),
			invalidChar()) != _table.end()) 
		throw std::domain_error(gettext("invalid table name"));
	table = _table;
}

void Search::setOrderColumn(const std::string& _col)
{
	if(_col.empty() || std::find_if(_col.begin(), _col.end(),
			invalidChar()) != _col.end()) 
		throw std::domain_error(gettext("invalid order column"));
	order_col = _col;
}

void Search::setOrderAsc(const bool _order_asc)
{
	order_asc = _order_asc;
}

void Search::setOwner(const unsigned int _oid)
{
	oid = _oid;
}

void Search::setContext(const unsigned int _cid)
{
	cid = _cid;
}

void Search::setContext(const std::string& _ctxname)
{
	cid = CachedContext(_ctxname)->getId();
}

void Search::setContext(const CachedContext& _ctx)
{
	cid = _ctx->getId();
}

void Search::setReadLevel(const unsigned int _lev)
{
	read_level = _lev;
}

void Search::setLimit(const unsigned int _limit)
{
	limit = _limit;
}

void Search::genSearchString()
{
	if(table.empty())
		throw std::domain_error(gettext("no table defined for search"));

	search_string = "SELECT id FROM " + table + " WHERE read_level <= :Qread_level";
	if(cid)
		search_string += " AND cid = :Qcid";
	if(oid)
		search_string += " AND oid = :Qoid";
	search_string += " AND id > 0 ORDER BY " + order_col;
	if(order_asc)
		search_string += " ASC";
	else
		search_string += " DESC";
	if(limit)
		search_string += " LIMIT :Qlimit";
}

void Search::genSearchKey()
{
	if(search_string.empty())
		genSearchString();

	std::ostringstream ostr;
	ostr << cid << oid << limit << read_level << search_string;
	search_key = ostr.str();
}

void Search::dbSearch()
{
	if(search_string.empty())
		genSearchString();

	try
	{
		Nexus& nx = Nexus::getInstance();
		dbconn conn = nx.dbConnection();

		tntdb::Statement q_ids = conn.prepare(search_string);
		if(cid)
			q_ids.setUnsigned("Qcid", cid);
		if(oid)
			q_ids.setUnsigned("Qoid", oid);
		if(limit)
			q_ids.setUnsigned("Qlimit", limit);
		if(!read_level)
			read_level = 2;
		q_ids.setUnsigned("Qread_level", read_level);

		tntdb::Result res_ids = q_ids.select();
		for(tntdb::Result::const_iterator it = res_ids.begin();
			it != res_ids.end(); ++it)
		{
			tntdb::Row row_ids = *it;
			if(!row_ids.empty() && !row_ids[0].isNull())
				results.push_back(row_ids[0].getUnsigned());
		}
	}
	catch(const std::exception& er_search)
        {
		log_error(er_search.what());
	}
}

const std::vector<unsigned int>& Search::getResults()
{
	if(results.empty())
		dbSearch();
	return results;
}

//end Search

} //namespace vagra
