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

#include <stdexcept>
#include <libintl.h>
#include <algorithm>

#include <tntdb/result.h>
#include <tntdb/row.h>
#include <cxxtools/log.h>

#include <vagra/nexus.h>
#include <vagra/article/articlecache.h>

namespace vagra
{

//begin ArticleCache

ArticleCache::ArticleCache()
{
	updateTagsum();
	updateMTime();
}

ArticleCache& ArticleCache::getInstance()
{
                static ArticleCache* volatile instance = NULL;
                static cxxtools::Mutex inst_mutex;

                cxxtools::membar_read();
                if(!instance)
                {
                        cxxtools::MutexLock lock(inst_mutex);
                        cxxtools::membar_read();
                        if(!instance)
                        {
                                ArticleCache* _tmp = new ArticleCache();
                                cxxtools::membar_write();
                                instance = _tmp;
                        }
                }
                return *instance;
}

unsigned int ArticleCache::getIdByTitle(const std::string& art_title)
{
        cxxtools::ReadLock rlock(id_map_mutex);
        std::map<std::string, unsigned int>::iterator it = id_map.find(art_title);
        if(it == id_map.end())
        {
                rlock.unlock();
                try
                {
                        unsigned int id = getArticleIdByTitle(art_title);
                        if(id)
                        {
                                cxxtools::WriteLock wlock(id_map_mutex);
                                id_map.insert(std::pair<std::string,unsigned int>(art_title,id));
                        }
                        return id;
                }
                catch(const std::exception& er_db)
                {
                        log_error(er_db.what());
                }
        }
        else
        {
                return(*it).second;
        }
        return 0;
}

tsum ArticleCache::getTagsum()
{
        cxxtools::ReadLock rlock(tsum_mutex);
        return tagsum;
}

void ArticleCache::updateMTime()
{
        try
        {
                Nexus& nx = Nexus::getInstance();
                dbconn conn = nx.dbConnection();

                tntdb::Statement q_mtime = conn.prepare(
                        "SELECT mtime FROM states");
                tntdb::Row row_mtime = q_mtime.selectRow();
                if(row_mtime[0].isNull())
                        throw std::domain_error(gettext("mtime in empty state"));

                cxxtools::WriteLock wlock(mtime_mutex);
                mtime = row_mtime[0].getDatetime();
        }
        catch(const std::exception& er_mtime)
        {
                log_error(er_mtime.what());
        }
}

vdate ArticleCache::getMTime()
{
        cxxtools::ReadLock rlock(mtime_mutex);
        return mtime;
}

void ArticleCache::updateTagsum()
{
        try
        {
                tsum tmptagsum;
                Nexus& nx = Nexus::getInstance();
                dbconn conn = nx.dbConnection();
                tntdb::Statement q_tags = conn.prepare(
                        "SELECT tag, count(tag) from tags GROUP by tag ORDER by count DESC;");
                tntdb::Result res_tags = q_tags.select();
                for(tntdb::Result::const_iterator it = res_tags.begin();
                        it != res_tags.end(); ++it)
                {
                        tntdb::Row row_tags = *it;
                        if(!row_tags[0].isNull() && !row_tags[1].isNull())
                        {
                                tmptagsum.push_back(std::pair<std::string, unsigned int> (row_tags[0].getString(), row_tags[1].getUnsigned()));
                        }
                }

                cxxtools::WriteLock wlock(tsum_mutex);
                tagsum.swap(tmptagsum);
        }
        catch(const std::exception& er_tsum)
        {
                log_error(er_tsum.what());
        }
}


//end ArticleCache

unsigned int cachedGetArticleIdByTitle(const std::string& title)
{
	ArticleCache& art_cache = ArticleCache::getInstance();
	return art_cache.getIdByTitle(title);
}

tsum getTagsum()
{
        ArticleCache& art_cache = ArticleCache::getInstance();
        return art_cache.getTagsum();
}

vdate updated()
{
        ArticleCache& art_cache = ArticleCache::getInstance();
        return art_cache.getMTime();
}

} //namespace vagra
