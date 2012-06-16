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
 * contenting executable to be covered by the GNU General Public
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

#include <vagra/config.h>
#include <vagra/article/tagsumpublic.h>

namespace vagra
{

//begin ArticleTagsPublicSearch
ArticleTagsPublicSearch::ArticleTagsPublicSearch()
	: BaseSearchVPSU("SELECT tag, count(tag) from tags, articles WHERE tags.art_id = articles.id AND"
			" articles.cid = (SELECT id FROM context WHERE name = 'public') AND"
		        " articles.read_level <= 2 GROUP by tag ORDER by count DESC LIMIT " + getConfig("article", "tagsum_size", std::string("16")) + ";")
{ }

//end ArticleTagsPublicSearch
//begin ArticleTagsPublic

ArticleTagsPublic::ArticleTagsPublic()
	: tagsum(ActiveCache<ArticleTagsPublicSearch>::getResult())
{ }

ArticleTagsPublic::const_iterator ArticleTagsPublic::begin() const
{
	return tagsum->begin();
}

ArticleTagsPublic::const_iterator ArticleTagsPublic::end() const
{
	return tagsum->end();
}

void ArticleTagsPublic::updateResult()
{
	ActiveCache<ArticleTagsPublicSearch>::updateResult();
}

//end ArticleTagsPublic

} //namespace vagra
