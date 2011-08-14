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

#include <vagra/nexus.h>
#include <vagra/article/articlecache.h>
#include <vagra/article/articlepage.h>

namespace vagra
{

ArtPage::ArtPage (const std::vector<unsigned int>& cont_ids, unsigned int arg_page, unsigned int arg_amount)
{
	if(arg_amount == 0)
	{
		Nexus& nx = Nexus::getInstance();
		arg_amount = nx.getConfig("article_page_size", nx.getPageSize());
	}
	Init(cont_ids, arg_page, arg_amount);
}

ArtPage::ArtPage(unsigned int arg_page, unsigned int arg_amount)
{
	ArticleCache& art_cache = ArticleCache::getInstance();
	if(arg_amount == 0)
	{
		Nexus& nx = Nexus::getInstance();
		arg_amount = nx.getConfig("article_page_size", nx.getPageSize());
	}
	Init(art_cache.getInv(), arg_page, arg_amount);
}

} //namespace vagra
