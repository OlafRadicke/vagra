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
