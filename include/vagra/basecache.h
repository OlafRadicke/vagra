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
#include <vagra/baseobject.h>
#include <vagra/nexus.h>

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
	BaseCache()
		: cache_inst(this->getCacheSize()) {}

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

	SharedObject get(const unsigned int id)
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
				SharedObject new_obj( new Object(id));
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
						obj = get(0);
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

		return obj;
	}

	void clear(unsigned int id)
	{
		cxxtools::MutexLock lock(mutex);
		cache_inst.erase(id);
	}

};

} //namespace vagra

#endif // VARGA_BASECACHE_H
