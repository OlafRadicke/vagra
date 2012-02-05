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

#ifndef VARGA_BASECACHE_H
#define VARGA_BASECACHE_H

#include <string>
#include <stdexcept>
#include <libintl.h>

#include <cxxtools/cache.h>
#include <cxxtools/noncopyable.h>
#include <cxxtools/smartptr.h>
#include <cxxtools/mutex.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <cxxtools/membar.h>

#include <vagra/types.h>
#include <vagra/nexus.h>
#include <vagra/resultcache.h>

namespace vagra
{

/* Base template for vagra Cache objects. 
 *  - Designed to use as base class template.
 *  - Classes that inherit from this class will need to define
 *    an instantination function.
 *  - For direct instantination use Cache defined in cache.h 
 */


template <typename Object>
class BaseCache : private cxxtools::NonCopyable
{
   public:
	typedef cxxtools::SmartPtr<const Object, cxxtools::ExternalAtomicRefCounted> SharedObject;
	typedef typename cxxtools::Cache<std::string, SharedObject>::size_type size_type;
	log_define("vagra")

    protected:
	ResultCache<Object>& result_cache;
	BaseCache()
       		: result_cache(ResultCache<Object>::getInstance()),
		  cache_inst(this->getCacheSize()) {}

    private:
	virtual unsigned int getCacheSize()
	{
		Nexus& nx = Nexus::getInstance();
		return nx.getCacheSize();
	}

	cxxtools::Cache<unsigned int, SharedObject> cache_inst;
	cxxtools::Mutex mutex;

    public:
	virtual ~BaseCache() {}
	void configure(const int cache_inst_size)
	{
		cxxtools::MutexLock lock(mutex);
		cache_inst.setMaxElements(cache_inst_size);
	}

	size_type size()
	{
		cxxtools::MutexLock lock(mutex);
		return cache_inst.size();
	}

	size_type getMaxElements()
	{
		cxxtools::MutexLock lock(mutex);
		return cache_inst.getMaxElements();
	}

	SharedObject get(const unsigned int id, const unsigned int _aid = 0) //assume 0 as anonymous
	{
		SharedObject obj;
		std::pair<bool, SharedObject> rdata;

		cxxtools::MutexLock lock(mutex);
		rdata = cache_inst.getx(id);
		lock.unlock();

		if(rdata.first)
		{
			obj = rdata.second;
		}
		else
		{
			try
			{
				SharedObject new_obj( new Object(id,1)); //FIXME assume 1 as admin
				if(*new_obj)
				{
					cxxtools::MutexLock lock(mutex);
					cache_inst.put(id, new_obj);
					lock.unlock();
					obj = new_obj;
				}
				else
				{
					if(id != 0)
						obj = get(0,_aid);
				}
			}
			catch(const std::exception& er_obj)
			{
				log_error(er_obj.what());
				throw std::domain_error(gettext("could not read object"));
			}
		}
		if(!obj)
			throw std::domain_error(gettext("failed to read any object"));
		if(_aid != 1 && obj->getAuthLevel(_aid) < obj->getReadLevel()) //FIXME asumme 1 as admin
			throw std::domain_error(gettext("insufficient privileges"));

		return obj;
	}

	void clear(unsigned int id)
	{
		cxxtools::MutexLock lock(mutex);
		cache_inst.erase(id);
		result_cache.clear();
	}

};

} //namespace vagra

#endif // VARGA_BASECACHE_H
