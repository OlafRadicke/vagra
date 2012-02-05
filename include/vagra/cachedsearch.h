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

#ifndef VARGA_CACHEDSEARCH_H
#define VARGA_CACHEDSEARCH_H

#include <string>

#include <vagra/search.h>
#include <vagra/resultcache.h>

namespace vagra
{

template <typename Object, class SearchImpl = Search>
class CachedSearch : public SearchImpl
{
	const std::vector<unsigned int>& cacheSearch()
	{
		if(SearchImpl::table.empty())
			SearchImpl::table = Object().getTable();
		if(SearchImpl::search_key.empty())
			SearchImpl::genSearchKey();

		ResultCache<Object>& rc = ResultCache<Object>::getInstance();
		std::pair<bool, typename ResultCache<Object>::SharedResults> _res(rc.get(SearchImpl::search_key));

		if(_res.first)
			SearchImpl::results = *(_res.second);
		else
		{
			SearchImpl::dbSearch();
			rc.put(SearchImpl::search_key, SearchImpl::results);
		}
	        return SearchImpl::results;
	}

    public:
	const std::vector<unsigned int>& getResults()
	{
		if(SearchImpl::results.empty())
			cacheSearch();
		return SearchImpl::results;
	}

	/* allow manually set search_key if genSearchKey() is not appropriate */
	void setSearchKey(const std::string& _key)
	{
		SearchImpl::search_key = _key;
	}
};

} //namespace vagra

#endif // VARGA_CACHEDSEARCH_H
