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

#ifndef VARGA_BASESEARCHVPSU_H
#define VARGA_BASESEARCHVPSU_H

#include <vector>
#include <utility>

#include <tntdb/result.h>
#include <tntdb/row.h>

#include <cxxtools/smartptr.h>

namespace vagra
{

/* stores results of the dbquery "search" into a vector of pair<string,unsigned>.
 * to be used as baseclass object for searches in conjunction with ActiveCache.
 */

class BaseSearchVPSU
{
	const std::string search;

	BaseSearchVPSU() {}

    protected:
	explicit BaseSearchVPSU(const std::string&);

    public:
	typedef std::vector<std::pair<std::string, unsigned int> > Result;
	typedef cxxtools::SmartPtr<Result,
		cxxtools::ExternalAtomicRefCounted> SharedResult;

	virtual ~BaseSearchVPSU() {}

	SharedResult getResult();
};

} //namespace vagra

#endif // VARGA_BASESEARCHVPSU_H
