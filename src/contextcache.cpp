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

#include <cxxtools/log.h>
#include <cxxtools/loginit.h>

#include <vagra/contextcache.h>

namespace vagra
{

//begin ContextCache

ContextCache& ContextCache::getInstance()
{
	static ContextCache* volatile instance = NULL;
	static cxxtools::Mutex inst_mutex;

	cxxtools::membar_read();
	if(!instance)
	{
		cxxtools::MutexLock lock(inst_mutex);
		cxxtools::membar_read();
		if(!instance)
		{
			ContextCache* _tmp = new ContextCache();
			cxxtools::membar_write();
			instance = _tmp;
		}
	}
	return *instance;
}

unsigned int ContextCache::getIdByName(const std::string& _name)
{
        cxxtools::ReadLock rlock(id_map_mutex);
        std::map<std::string, unsigned int>::iterator it = id_map.find(_name);
        if(it == id_map.end())
        {
                rlock.unlock();
                try
                {
                        unsigned int _id = getContextIdByName(_name);
                        if(_id)
                        {
                                cxxtools::WriteLock wlock(id_map_mutex);
                                id_map.insert(std::pair<std::string,unsigned int>(_name, _id));
                        }
                        return _id;
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

//end ContextCache

unsigned int cachedGetContextIdByName(const std::string& _name)
{
	ContextCache& ctx_cache = ContextCache::getInstance();
	return ctx_cache.getIdByName(_name);
}

} //namespace vagra
