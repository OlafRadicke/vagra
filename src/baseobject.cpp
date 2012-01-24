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

BaseObject::BaseObject(const std::string& _tablename, const unsigned int _id, const unsigned int _aid)
	: oid(0), gid(0), read_level(126), add_level(126), write_level(126), id(_id), tablename(_tablename)
{
        try
	{
                Nexus& nx = Nexus::getInstance();
                dbconn conn = nx.dbConnection();

		tntdb::Statement q_obj = conn.prepare(
                        "SELECT oid, gid, read_level, add_level, write_level, ctime, mtime"
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
                        add_level = row_obj[3].getUnsigned();	
                if(!row_obj[4].isNull())
                        write_level = row_obj[4].getUnsigned();	
		if(!row_obj[5].isNull())
			ctime = row_obj[5].getDatetime();
		if(!row_obj[6].isNull())
			mtime = row_obj[6].getDatetime();
        }
        catch(const std::exception& er_obj)
        {
                log_error(er_obj.what());
                throw std::domain_error(gettext("could not init BaseObject"));
        }
	if(getAuthLevel(_aid) < read_level)
		throw std::domain_error(gettext("insufficient privileges"));
}

BaseObject::operator bool() const
{
	return id;
}

void BaseObject::dbCommitBase(dbconn& conn, const unsigned int _aid)
{
	if(id) // id!=null, update. otherwise insert
	{
		if(getAuthLevel(_aid) < write_level)
			throw std::domain_error(gettext("insufficient privileges"));

		conn.prepare("UPDATE " + tablename + " SET oid = :Ioid, gid = :Igid,"
                	" read_level = :Iread_level, add_level = :Iadd_level,"
		        " write_level = :Iwrite_level, mtime = now() WHERE id = :Iid")
		.setUnsigned("Ioid", oid)
		.setUnsigned("Igid", gid)
		.setUnsigned("Iread_level", read_level)
		.setUnsigned("Iadd_level", add_level)
		.setUnsigned("Iwrite_level", write_level)
		.setUnsigned("Iid", id)
		.execute();
	}
	else
	{
		//if authId doesn't reach write_level, adding new objects might still be desired
		if(getAuthLevel(_aid) < write_level && getAuthLevel(_aid) < add_level)
			throw std::domain_error(gettext("insufficient privileges"));

		tntdb::Statement ins_obj = conn.prepare("INSERT INTO "
			+ tablename + " (oid, gid, read_level, add_level, write_level)"
                	" VALUES (:Ioid, :Igid, :Iread_level, :Iadd_level, :Iwrite_level)"
		        " RETURNING id");		// get id from fresh created obj
		ins_obj.setUnsigned("Ioid", oid);
		ins_obj.setUnsigned("Igid", gid);
		ins_obj.setUnsigned("Iread_level", read_level);
		ins_obj.setUnsigned("Iadd_level", add_level);
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
	add_level = 126;
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

const unsigned char BaseObject::getAddLevel() const
{
	return add_level;
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

const unsigned char BaseObject::getAuthLevel(const unsigned int _aid) const
{
        unsigned char _auth_level(2);

        if(_aid) {
                _auth_level += 6;
                if(_aid == gid)
                        _auth_level += 14;
                if(_aid == oid)
                        _auth_level += 30;
                if(_aid == 4) //assume 4 as groupadmin //FIXME
                        _auth_level += 62;
                if(_aid == 1) //assume 1 as admin //FIXME
                        _auth_level += 126;
        }
        return _auth_level;
}

void BaseObject::setOwner(const unsigned int _oid, const unsigned int _aid)
{	
	// allow taking ownership of anonymous owned objects if write_level is low enough
	if(oid == 0 && write_level <= 30)
		oid = _oid;
	else
	{
		if(getAuthLevel(_aid) < write_level)
			throw std::domain_error(gettext("insufficient privileges"));
		oid = _oid;
	}
}

void BaseObject::setGroup(const unsigned int _gid, const unsigned int _aid)
{
	if(getAuthLevel(_aid) < write_level)
		throw std::domain_error(gettext("insufficient privileges"));
	gid = _gid;
}

void BaseObject::setReadLevel(const unsigned char _lev, const unsigned int _aid)
{
	if(getAuthLevel(_aid) < write_level)
		throw std::domain_error(gettext("insufficient privileges"));
	read_level = _lev;
}

void BaseObject::setAddLevel(const unsigned char _lev, const unsigned int _aid)
{
	if(getAuthLevel(_aid) < write_level)
		throw std::domain_error(gettext("insufficient privileges"));
	add_level = _lev;
}

void BaseObject::setWriteLevel(const unsigned char _lev, const unsigned int _aid)
{
	if(getAuthLevel(_aid) < write_level)
		throw std::domain_error(gettext("insufficient privileges"));
	write_level = _lev;
}

//end BaseObject

} //namespace vagra
