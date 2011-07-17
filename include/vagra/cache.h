#ifndef VARGA_CACHE_H
#define VARGA_CACHE_H

#include <vagra/basecache.h>

namespace vagra
{

/* Cache for vagra objects.
 *  - Objects need a constructor that takes one unsigned int as parameter
 *  - Objects need a valid bool() operator
 *  - If 0 is given as costructor parameter, it shall generate an special object,
 *    that acts as replacement for Object that's bool operator return false
 */


template <typename Object>
class Cache : public BaseCache<Object>
{
    public:
	static Cache& getInstance()
	{
		static Cache* volatile instance = NULL;
		static cxxtools::Mutex inst_mutex;

		cxxtools::membar_read();
		if(!instance)
		{
			cxxtools::MutexLock lock(inst_mutex);
			cxxtools::membar_read();
			if(!instance)
			{
				Cache* _tmp = new Cache();
				cxxtools::membar_write();
				instance = _tmp;
			}
		}
		return *instance;
	}
};

} //namespace vagra

#endif // VARGA_CACHE_H
