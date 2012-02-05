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
	: ctx(0), oid(0), read_level(126), add_level(126), write_level(126), id(_id), tablename(_tablename)
{
        try
	{
                Nexus& nx = Nexus::getInstance();
                dbconn conn = nx.dbConnection();

		tntdb::Statement q_obj = conn.prepare(
                        "SELECT oid, cid, read_level, add_level, write_level, ctime, mtime"
                                " FROM " + tablename + " WHERE id = :Qid");
                q_obj.setUnsigned("Qid", id);
                tntdb::Row row_obj = q_obj.selectRow();
                if(row_obj.empty())
                        throw std::domain_error(gettext("couldn't read object form db"));
                if(!row_obj[0].isNull())
                        oid = row_obj[0].getUnsigned();
                if(!row_obj[1].isNull())
                        ctx = CachedContext(row_obj[1].getUnsigned());
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

		conn.prepare("UPDATE " + tablename + " SET oid = :Ioid, cid = :Icid,"
                	" read_level = :Iread_level, add_level = :Iadd_level,"
		        " write_level = :Iwrite_level, mtime = now() WHERE id = :Iid")
		.setUnsigned("Ioid", oid)
		.setUnsigned("Icid", ctx->getId())
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
			+ tablename + " (oid, cid, read_level, add_level, write_level)"
                	" VALUES (:Ioid, :Icid, :Iread_level, :Iadd_level, :Iwrite_level)"
		        " RETURNING id");		// get id from fresh created obj
		ins_obj.setUnsigned("Ioid", oid);
		ins_obj.setUnsigned("Icid", ctx->getId());
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
	ctx = CachedContext(0);
	read_level = 126;
	add_level = 126;
	write_level = 126;
	ctime = vdate();
	mtime = vdate();
}

const CachedContext& BaseObject::getContext() const
{
	return ctx;
}

const unsigned int BaseObject::getId() const
{
	return id;
}

const unsigned int BaseObject::getOwner() const
{
	return oid;
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

const std::string& BaseObject::getTable() const
{
	return tablename;
}

const unsigned char BaseObject::getAuthLevel(const unsigned int _aid) const
{
        unsigned char _auth_level(2);

        if(_aid) {
                _auth_level += 6;
                if(_aid == oid)
                        _auth_level += 30;
                if(_aid == 1) //assume 1 as admin //FIXME
                        _auth_level += 126;
		_auth_level += ctx->getAuthLevel(_aid);
        }
        return _auth_level;
}

void BaseObject::setContext(const CachedContext& _ctx, const unsigned int _aid)
{
	if(!id) //new object if false
	{
		if(_ctx->getAuthLevel(_aid) < _ctx->getAddLevel()
			&& _ctx->getAddLevel() > 2)
			throw std::domain_error(gettext("insufficient privileges on target context"));
	}
	else 
	{
		if(getAuthLevel(_aid) < write_level)
			throw std::domain_error(gettext("insufficient privileges"));

		if(_ctx->getAuthLevel(_aid) < _ctx->getAddLevel()
			&& _ctx->getAddLevel() > 2)
			throw std::domain_error(gettext("insufficient privileges on target context"));
	}
	ctx = _ctx;
	oid = _aid;
	read_level = ctx->getReadLevel();
	add_level = ctx->getAddLevel();
	write_level = ctx->getWriteLevel();
}

void BaseObject::setContext(const std::string& _name, const unsigned int _aid)
{
	CachedContext _ctx(_name);
	setContext(_ctx, _aid);
}

void BaseObject::setContext(const unsigned int _cid, const unsigned int _aid)
{
	CachedContext _ctx(_cid);
	setContext(_ctx, _aid);
}

void BaseObject::setOwner(const unsigned int _oid, const unsigned int _aid)
{	
	if(getAuthLevel(_aid) < write_level)
		throw std::domain_error(gettext("insufficient privileges"));
	oid = _oid;
}

void BaseObject::setReadLevel(const unsigned char _lev, const unsigned int _aid)
{
	unsigned char _priv = getAuthLevel(_aid);
	if(_priv < write_level)
		throw std::domain_error(gettext("insufficient privileges"));
	if(_lev > 62 && _lev > _priv)
		throw std::domain_error(gettext("can't raise privileges above 62"));
	if(_lev > 126)
		throw std::domain_error(gettext("can't raise privileges above 126"));
	read_level = _lev;
}

void BaseObject::setAddLevel(const unsigned char _lev, const unsigned int _aid)
{
	unsigned char _priv = getAuthLevel(_aid);
	if(_priv < write_level)
		throw std::domain_error(gettext("insufficient privileges"));
	if(_lev > 62 && _lev > _priv)
		throw std::domain_error(gettext("can't raise privileges above 62"));
	if(_lev > 126)
		throw std::domain_error(gettext("can't raise privileges above 126"));
	add_level = _lev;
}

void BaseObject::setWriteLevel(const unsigned char _lev, const unsigned int _aid)
{
	unsigned char _priv = getAuthLevel(_aid);
	if(_priv < write_level)
		throw std::domain_error(gettext("insufficient privileges"));
	if(_lev > 62 && _lev > _priv)
		throw std::domain_error(gettext("can't raise privileges above 62"));
	if(_lev > 126)
		throw std::domain_error(gettext("can't raise privileges above 126"));
	write_level = _lev;
}

//end BaseObject

} //namespace vagra
