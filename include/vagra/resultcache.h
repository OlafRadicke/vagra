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

#ifndef VARGA_RESULTCACHE_H
#define VARGA_RESULTCACHE_H

#include <string>
#include <vector>

#include <cxxtools/cache.h>
#include <cxxtools/noncopyable.h>
#include <cxxtools/smartptr.h>
#include <cxxtools/mutex.h>
#include <cxxtools/membar.h>

#include <vagra/nexus.h>

namespace vagra
{

template <typename Object>
class ResultCache : private cxxtools::NonCopyable
{
    public:
	typedef cxxtools::SmartPtr<const std::vector<unsigned int>, 
		cxxtools::ExternalAtomicRefCounted> SharedResults;

	static ResultCache& getInstance()
	{
		static ResultCache* volatile instance = NULL;
		static cxxtools::Mutex inst_mutex;

		cxxtools::membar_read();
		if(!instance)
		{
			cxxtools::MutexLock lock(inst_mutex);
			cxxtools::membar_read();
			if(!instance)
			{
				ResultCache* _tmp = new ResultCache();
				cxxtools::membar_write();
				instance = _tmp;
			}
		}
		return *instance;
	}

	std::pair<bool, SharedResults> get(const std::string& _key)
	{
		cxxtools::MutexLock lock(mutex);
		return cache_inst.getx(_key);
	}

	void put(const std::string& _key, const std::vector<unsigned int>& _res)
	{
		cxxtools::MutexLock lock(mutex);
		cache_inst.put(_key, SharedResults(new std::vector<unsigned int>(_res)));
	}

	void clear()
	{
		cache_inst.clear();
	}

    private:
	cxxtools::Cache<std::string, SharedResults> cache_inst;
	cxxtools::Mutex mutex;

	ResultCache ()
		: cache_inst(this->getCacheSize()) {}

	unsigned int getCacheSize()
	{
		Nexus& nx = Nexus::getInstance();
		return nx.getCacheSize();
	}
};

} //namespace vagra

#endif // VARGA_RESULTCACHE_H
