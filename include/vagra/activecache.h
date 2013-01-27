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

#ifndef VARGA_ACTIVECACHE_H
#define VARGA_ACTIVECACHE_H

#include <cxxtools/noncopyable.h>
#include <cxxtools/smartptr.h>
#include <cxxtools/mutex.h>
#include <cxxtools/condition.h>
#include <cxxtools/thread.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <cxxtools/membar.h>

#include <vagra/exception.h>

namespace vagra
{

template <typename SearchFunc>
class ActiveCache : private cxxtools::NonCopyable
{	
	log_define("vagra")

    public:
	typedef typename SearchFunc::Result Result;
	typedef typename SearchFunc::SharedResult SharedResult;
	typedef typename Result::const_iterator const_iterator;

   private:
	ActiveCache()
		: result(search_func.getResult()),
		  need_update(true)
	{
		UpdateThread *thread = new UpdateThread();
		thread->start();
	}

	SearchFunc search_func;
	SharedResult result;
	
        cxxtools::ReadWriteMutex ac_mutex;
        cxxtools::Mutex update_mutex;
	bool need_update;

        cxxtools::Mutex cond_mutex;
	cxxtools::Condition cond;

	class UpdateThread : public cxxtools::DetachedThread
	{
	    protected:
		void run()
		{ 
			ActiveCache& ac = ActiveCache::getInstance();
			ac.updateImpl();
		}
	};

    public:
	static ActiveCache& getInstance()
	{
		static ActiveCache* volatile instance = NULL;
		static cxxtools::Mutex inst_mutex;

		cxxtools::membar_read();
		if(!instance)
		{
			cxxtools::MutexLock lock(inst_mutex);
			cxxtools::membar_read();
			if(!instance)
			{
				ActiveCache* _tmp = new ActiveCache();
				cxxtools::membar_write();
				instance = _tmp;
			}
		}
		return *instance;
	}

	static SharedResult getResult()
	{
		ActiveCache& ac = getInstance();
		cxxtools::ReadLock rlock(ac.ac_mutex);
		return ac.result;
	}

	static void updateResult()
	{
		ActiveCache& ac = getInstance();
		ac.triggerUpdate();
	}

	void triggerUpdate()
	{
		try {
			cxxtools::MutexLock lock(update_mutex);
			need_update = true;
			cond.signal();
		}
		catch(const std::exception& er_ac)
		{
			log_error(er_ac.what());
		}
	}

	void updateImpl()
	{
		SharedResult _result(new Result);
		while(true)
		{
			try
			{
				cxxtools::MutexLock cond_lock(cond_mutex);
				cond.wait(cond_lock);

				cxxtools::MutexLock update_lock(update_mutex);
				if(need_update)
				{
					need_update = false;
					update_lock.unlock();
					_result = search_func.getResult();
					cxxtools::WriteLock wlock(ac_mutex);
					result = _result;
				}
			}
			catch(const Exception&)
			{
				throw;
			}
			catch(const std::exception& er_ac)
			{
				log_error(er_ac.what());
			}
		}
	}
};

} //namespace vagra

#endif // VARGA_ACTIVECACHE_H
