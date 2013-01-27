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
#include <algorithm>
#include <sstream>

#include <vagra/exception.h>
#include <vagra/utils.h>
#include <vagra/article/articlesearch.h>

namespace vagra
{

//begin Search

void ArticleTagSearch::setTag(const std::string& _tag)
{
	if(_tag.empty() || std::find_if(_tag.begin(), _tag.end(),
			invalidChar()) != _tag.end()) 
		throw InvalidValue(gettext("invalid tag"));
	tag = _tag;
}

void ArticleTagSearch::genSearchString()
{
	if(tag.empty())
		throw InvalidValue(gettext("no tag defined for search"));

	search_string = "SELECT id FROM articles, tags WHERE tags.art_id = articles.id AND tags.tag = '"
			+ tag + "' AND read_level <= :Qread_level";
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

//end Search

} //namespace vagra
