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

#ifndef VARGA_ARTICLECACHE_H
#define VARGA_ARTICLECACHE_H

#include <map>
#include <vector>

#include <cxxtools/mutex.h>

#include <vagra/types.h>
#include <vagra/basecache.h>
#include <vagra/date.h>

#include <vagra/article/article.h>

namespace vagra
{

typedef std::vector<std::pair<std::string, unsigned int> > tsum;

class ArticleCache: public BaseCache<Article>
{
	ArticleCache();

        unsigned int getCacheSize()
        {
                Nexus& nx = Nexus::getInstance();
                return nx.getConfig("art_cache_size", nx.getCacheSize());
        }

        std::map<std::string, unsigned int> id_map;
        std::vector<unsigned int> inventory;
	tsum tagsum;
	vdate mtime;

        cxxtools::ReadWriteMutex id_map_mutex;
	cxxtools::ReadWriteMutex tsum_mutex;
        cxxtools::ReadWriteMutex id_inv_mutex;
	cxxtools::ReadWriteMutex mtime_mutex;

	void initInv();

    public:
	typedef std::vector<unsigned int>::size_type size_type;

        static ArticleCache& getInstance();
        unsigned int getIdByTitle(const std::string&);

	std::vector<unsigned int> getInv();
	std::vector<unsigned int> getInv(unsigned int, unsigned int);
	size_type invSize() const;
	tsum getTagsum();
	vdate getMTime();

	void invAdd(unsigned int);
	void invRemove(unsigned int);
	void updateTagsum();
	void updateMTime();
};

typedef ArticleCache::SharedObject SharedArticle;

unsigned int cachedGetArticleIdByTitle(const std::string&);
tsum getTagsum();
ArticleCache::size_type getArticleAmount();

std::vector<unsigned int> searchArtByTag(const std::string&);

vdate updated();

} //namespace vagra

#endif // VARGA_ARTICLECACHE_H
