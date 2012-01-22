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

#include <libintl.h>
#include <stdexcept>

#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <tntdb/row.h>

#include <vagra/nexus.h>
#include <vagra/baseobject.h>

namespace vagra
{

log_define("vagra")

//begin BaseObject

BaseObject::BaseObject(const std::string& _tablename, const unsigned int _id)
	: tablename(_tablename), id(_id), oid(0), gid(0), read_level(126), write_level(126)
{
        try
	{
                Nexus& nx = Nexus::getInstance();
                dbconn conn = nx.dbConnection();

		tntdb::Statement q_obj = conn.prepare(
                        "SELECT oid, gid, read_level, write_level, ctime, mtime"
                                " FROM " + tablename + " WHERE id = :Qid");
                q_obj.setUnsigned("Qid", id);
                tntdb::Row row_obj = q_obj.selectRow();
                if(row_obj.empty())
                        throw std::domain_error(gettext("couldn't read object form db"));
                if(!row_obj[0].isNull())
                        oid = row_obj[0].getUnsigned();
                if(!row_obj[1].isNull())
                        gid = row_obj[1].getUnsigned();
                if(!row_obj[2].isNull())
                        read_level = row_obj[2].getUnsigned();
                if(!row_obj[3].isNull())
                        write_level = row_obj[3].getUnsigned();	
		if(!row_obj[4].isNull())
			ctime = row_obj[4].getDatetime();
		if(!row_obj[5].isNull())
			mtime = row_obj[5].getDatetime();
        }
        catch(const std::exception& er_obj)
        {
                log_error(er_obj.what());
                throw std::domain_error(gettext("could not init BaseObject"));
        }
}

BaseObject::operator bool() const
{
	return id;
}

void BaseObject::dbCommitBase(dbconn& conn)
{
	if(id) // id!=null, update. otherwise insert
	{
		conn.prepare("UPDATE " + tablename + " SET oid = :Ioid, gid = :Igid,"
                	" read_level = :Iread_level, write_level = :Iwrite_level,"
		        " mtime = now() WHERE id = :Iid")
		.setUnsigned("Ioid", oid)
		.setUnsigned("Igid", gid)
		.setUnsigned("Iread_level", read_level)
		.setUnsigned("Iwrite_level", write_level)
		.setUnsigned("Iid", id)
		.execute();
	}
	else
	{
		tntdb::Statement ins_obj = conn.prepare("INSERT INTO "
			+ tablename + " (oid, gid, read_level, write_level)"
                	" VALUES (:Ioid, :Igid, :Iread_level, :Iwrite_level)"
		        " RETURNING id");		// get id from fresh created obj
		ins_obj.setUnsigned("Ioid", oid);
		ins_obj.setUnsigned("Igid", gid);
		ins_obj.setUnsigned("Iread_level", read_level);
		ins_obj.setUnsigned("Iwrite_level", write_level);
		tntdb::Row row_obj = ins_obj.selectRow();
		if(row_obj[0].isNull())
                        throw std::domain_error(gettext("insert didn't retuned an id"));
                id = row_obj[0].getUnsigned();
	}
}

void BaseObject::clearBase()
{
	id = 0;
	oid = 0;
	gid = 0;
	read_level = 126;
	write_level = 126;
	ctime = vdate();
	mtime = vdate();
}

const unsigned int BaseObject::getId() const
{
	return id;
}

const unsigned int BaseObject::getOwner() const
{
	return oid;
}

const unsigned int BaseObject::getGroup() const
{
	return gid;
}

const unsigned char BaseObject::getReadLevel() const
{
	return read_level;
}

const unsigned char BaseObject::getWriteLevel() const
{
	return write_level;
}

const vdate& BaseObject::getCTime() const
{
	return ctime;
}

const vdate& BaseObject::getMTime() const
{
	return mtime;
}


void BaseObject::setOwner(unsigned int _oid)
{
	oid = _oid;
}

void BaseObject::setGroup(unsigned int _gid)
{
	gid = _gid;
}

void BaseObject::setReadLevel(unsigned char _lev)
{
	read_level = _lev;
}

void BaseObject::setWriteLevel(unsigned char _lev)
{
	write_level = _lev;
}

//end BaseObject

} //namespace vagra
