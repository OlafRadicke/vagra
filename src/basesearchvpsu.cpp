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

#include <stdexcept>

#include <tntdb/result.h>
#include <tntdb/row.h>

#include <cxxtools/log.h>
#include <cxxtools/loginit.h>

#include <vagra/basesearchvpsu.h>
#include <vagra/types.h>
#include <vagra/nexus.h>

namespace vagra
{

log_define("vagra")

//begin BaseSearchVPSU

BaseSearchVPSU::BaseSearchVPSU(const std::string& _search)
	: search(_search)
{ }

BaseSearchVPSU::SharedResult BaseSearchVPSU::getResult()
{
        try
        {
		SharedResult _result(new Result);
                Nexus& nx = Nexus::getInstance();
                dbconn conn = nx.dbConnection();
                tntdb::Statement q_search = conn.prepare(search);
                tntdb::Result res_search = q_search.select();
                for(tntdb::Result::const_iterator it = res_search.begin();
                        it != res_search.end(); ++it)
                {
                        tntdb::Row row_search = *it;
                        if(!row_search[0].isNull() && !row_search[1].isNull())
                        {
                                _result->push_back(std::pair<std::string, unsigned int> (row_search[0].getString(), row_search[1].getUnsigned()));
                        }
                }

		return _result;
        }
        catch(const std::exception& er_sf)
        {
                log_error(er_sf.what());
        }
	return SharedResult(new Result); //return empty Result if search fails due an excaption
}

//end BaseSearchVPSU

} //namespace vagra
