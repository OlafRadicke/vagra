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
#include <algorithm>

#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <tntdb/transaction.h>
#include <tntdb/result.h>
#include <tntdb/row.h>

#include <vagra/contextcache.h>
#include <vagra/nexus.h>

namespace vagra
{

log_define("vagra")

//begin Context

Context::Context(const std::string& _name, const unsigned int _aid)
{
	*this = Context(cachedGetContextIdByName(_name));
}

Context::Context(const unsigned int _id, const unsigned int _aid)
	: id(0), read_level(126), add_level(126), write_level(126)
{
        try
	{
                Nexus& nx = Nexus::getInstance();
                dbconn conn = nx.dbConnection();

		tntdb::Statement q_ctx = conn.prepare(
                        "SELECT name, read_level, add_level, write_level"
                                " FROM context WHERE id = :Qid");
                q_ctx.setUnsigned("Qid", _id);
                tntdb::Row row_ctx = q_ctx.selectRow();
                if(row_ctx.empty())
                        throw std::domain_error(gettext("couldn't read context form db"));
                if(!row_ctx[0].isNull())
                        name = row_ctx[0].getString();
                if(!row_ctx[1].isNull())
                        read_level = row_ctx[1].getUnsigned();
                if(!row_ctx[2].isNull())
                        add_level = row_ctx[2].getUnsigned();
                if(!row_ctx[3].isNull())
                        write_level = row_ctx[3].getUnsigned();	

		q_ctx = conn.prepare(
                        "SELECT member FROM context_admins WHERE id = :Qid");
                q_ctx.setUnsigned("Qid", _id);
		tntdb::Result res_ctx = q_ctx.select();
		for(tntdb::Result::const_iterator it = res_ctx.begin();
				it != res_ctx.end(); ++it)
		{
			row_ctx = *it;
			if(!row_ctx.empty() && !row_ctx[0].isNull())
				admin.push_back(row_ctx[0].getUnsigned());
		}

		q_ctx = conn.prepare(
                        "SELECT member FROM context_privileged WHERE id = :Qid");
                q_ctx.setUnsigned("Qid", _id);
		res_ctx = q_ctx.select();
		for(tntdb::Result::const_iterator it = res_ctx.begin();
				it != res_ctx.end(); ++it)
		{
			row_ctx = *it;
			if(!row_ctx.empty() && !row_ctx[0].isNull())
				privileged.push_back(row_ctx[0].getUnsigned());
		}

		q_ctx = conn.prepare(
                        "SELECT member FROM context_unprivileged WHERE id = :Qid");
                q_ctx.setUnsigned("Qid", _id);
		res_ctx = q_ctx.select();
		for(tntdb::Result::const_iterator it = res_ctx.begin();
				it != res_ctx.end(); ++it)
		{
			row_ctx = *it;
			if(!row_ctx.empty() && !row_ctx[0].isNull())
				unprivileged.push_back(row_ctx[0].getUnsigned());
		}
		id = _id;
        }
        catch(const std::exception& er_ctx)
        {
                log_error(er_ctx.what());
                throw std::domain_error(gettext("could not init context"));
        }
}

void Context::dbCommit(const unsigned int _aid)
{
	if(!_aid || (_aid != 1 //FIXME assume admin as 1
			&& !std::binary_search(admin.begin(), admin.end(), _aid)))
		throw std::domain_error(gettext("insufficient privileges"));

	try
	{
		Nexus& nx = Nexus::getInstance();
		dbconn conn = nx.dbConnection();

		tntdb::Transaction trans_ctx(conn);

		if(id != getContextIdByName(name))
			throw std::domain_error(gettext("name already used by other context"));

		if(id) // id!=null, update. otherwise insert
		{
			conn.prepare("UPDATE context SET name = :Iname,"
       		         	" read_level = :Iread_level, add_level = :Iadd_level,"
			        " write_level = :Iwrite_level WHERE id = :Iid")
			.setString("Iname", name)
			.setUnsigned("Iread_level", read_level)
			.setUnsigned("Iadd_level", add_level)
			.setUnsigned("Iwrite_level", write_level)
			.setUnsigned("Iid", id)
			.execute();
		}
		else
		{
			tntdb::Statement ins_obj = conn.prepare("INSERT INTO context"
				" (name, read_level, add_level, write_level)"
	                	" VALUES (:Iname, :Iread_level, :Iadd_level, :Iwrite_level)"
			        " RETURNING id");		// get id from fresh created obj
			ins_obj.setString("Iname", name);
			ins_obj.setUnsigned("Iread_level", read_level);
			ins_obj.setUnsigned("Iadd_level", add_level);
			ins_obj.setUnsigned("Iwrite_level", write_level);
			tntdb::Row row_ctx = ins_obj.selectRow();
			if(row_ctx.empty() || row_ctx[0].isNull())
				throw std::domain_error(gettext("insert didn't retuned an id"));

			id = row_ctx[0].getUnsigned();
		}

		conn.prepare("DELETE FROM context_admins WHERE id = :Qid")
			.setUnsigned("Qid", id)
			.execute();
		for(std::vector<unsigned int>::iterator it = admin.begin();
				it != admin.end(); it++)
		{
			conn.prepare("INSERT INTO context_admins (id, member)"
				" VALUES (:Iid, :Imember)")
				.setUnsigned("Iid", id)
				.setUnsigned("Imember", *it)
				.execute();
		}


		conn.prepare("DELETE FROM context_privileged WHERE id = :Qid")
			.setUnsigned("Qid", id)
			.execute();
		for(std::vector<unsigned int>::iterator it = privileged.begin();
				it != privileged.end(); it++)
		{
			conn.prepare("INSERT INTO context_privileged (id, member)"
				" VALUES (:Iid, :Imember)")
				.setUnsigned("Iid", id)
				.setUnsigned("Imember", *it)
				.execute();
		}

		conn.prepare("DELETE FROM context_unprivileged WHERE id = :Qid")
			.setUnsigned("Qid", id)
			.execute();
		for(std::vector<unsigned int>::iterator it = unprivileged.begin();
				it != unprivileged.end(); it++)
		{
			conn.prepare("INSERT INTO context_unprivileged (id, member)"
				" VALUES (:Iid, :Imember)")
				.setUnsigned("Iid", id)
				.setUnsigned("Imember", *it)
				.execute();
		}

		ContextCache& ctx_cache = ContextCache::getInstance();
		trans_ctx.commit();
		ctx_cache.clear(id);
	}
        catch(const std::exception& er_ctx)
        {
                log_error(er_ctx.what());
                throw std::domain_error(gettext("could commit context"));
        }
}

Context::operator bool() const
{
	return !name.empty();
}

Context::operator int() const
{
	return(id);
}

Context::operator unsigned int() const
{
	return(id);
}

const std::string& Context::getName() const
{
	return name;
}

const unsigned int Context::getId() const
{
	return id;
}

const unsigned char Context::getReadLevel() const
{
	return read_level;
}

const unsigned char Context::getAddLevel() const
{
	return add_level;
}

const unsigned char Context::getWriteLevel() const
{
	return write_level;
}

const unsigned char Context::getAuthLevel(const unsigned int _aid) const
{
	if(!_aid)
		return 0;

        unsigned char _auth_level(0);

	if(std::binary_search(unprivileged.begin(), unprivileged.end(), _aid))
		_auth_level = 14;
	if(std::binary_search(privileged.begin(), privileged.end(), _aid))
		_auth_level += 62;

        return _auth_level;
}

void Context::setReadLevel(const unsigned char _lev, const unsigned int _aid)
{
	if(!_aid || (_aid != 1 //FIXME assume admin as 1
			&& !std::binary_search(admin.begin(), admin.end(), _aid)))
		throw std::domain_error(gettext("insufficient privileges"));
	read_level = _lev;
}

void Context::setAddLevel(const unsigned char _lev, const unsigned int _aid)
{
	if(!_aid || (_aid != 1 //FIXME assume admin as 1
			&& !std::binary_search(admin.begin(), admin.end(), _aid)))
		throw std::domain_error(gettext("insufficient privileges"));
	add_level = _lev;
}

void Context::setWriteLevel(const unsigned char _lev, const unsigned int _aid)
{
	if(!_aid || (_aid != 1 //FIXME assume admin as 1
			&& !std::binary_search(admin.begin(), admin.end(), _aid)))
		throw std::domain_error(gettext("insufficient privileges"));
	write_level = _lev;
}

void Context::addUnprivileged(const unsigned int _uid, const unsigned int _aid)
{
	if(!_aid || (_aid != 1 //FIXME assume admin as 1
			&& !std::binary_search(admin.begin(), admin.end(), _aid)))
		throw std::domain_error(gettext("insufficient privileges"));

	if(!std::binary_search(unprivileged.begin(), unprivileged.end(), _uid))
		unprivileged.push_back(_uid);
}

void Context::addPrivileged(const unsigned int _uid, const unsigned int _aid)
{
	if(!_aid || (_aid != 1 //FIXME assume admin as 1
			&& !std::binary_search(admin.begin(), admin.end(), _aid)))
		throw std::domain_error(gettext("insufficient privileges"));

	if(!std::binary_search(privileged.begin(), privileged.end(), _uid))
		privileged.push_back(_uid);
}

void Context::addAdmin(const unsigned int _uid, const unsigned int _aid)
{
	if(!_aid || (_aid != 1 //FIXME assume admin as 1
			&& !std::binary_search(admin.begin(), admin.end(), _aid)))
		throw std::domain_error(gettext("insufficient privileges"));

	if(!std::binary_search(admin.begin(), admin.end(), _uid))
		admin.push_back(_uid);
}

void Context::removeUnprivileged(const unsigned int _uid, const unsigned int _aid)
{
	if(!_aid || (_aid != 1 //FIXME assume admin as 1
			&& !std::binary_search(admin.begin(), admin.end(), _aid)))
		throw std::domain_error(gettext("insufficient privileges"));

	std::vector<unsigned int>::iterator it = std::find(unprivileged.begin(), unprivileged.end(), _uid);
	if(it != unprivileged.end())
		unprivileged.erase(it);
}

void Context::removePrivileged(const unsigned int _uid, const unsigned int _aid)
{
	if(!_aid || (_aid != 1 //FIXME assume admin as 1
			&& !std::binary_search(admin.begin(), admin.end(), _aid)))
		throw std::domain_error(gettext("insufficient privileges"));

	std::vector<unsigned int>::iterator it = std::find(privileged.begin(), privileged.end(), _uid);
	if(it != privileged.end())
		privileged.erase(it);
}

void Context::removeAdmin(const unsigned int _uid, const unsigned int _aid)
{
	if(!_aid || (_aid != 1 //FIXME assume admin as 1
			&& !std::binary_search(admin.begin(), admin.end(), _aid)))
		throw std::domain_error(gettext("insufficient privileges"));

	std::vector<unsigned int>::iterator it = std::find(admin.begin(), admin.end(), _uid);
	if(it != admin.end())
		admin.erase(it);
}

//end Context

unsigned int getContextIdByName(const std::string& _name)
{
        try
        {
                Nexus& nx = Nexus::getInstance();
                dbconn conn = nx.dbConnection();
                return getContextIdByName(_name, conn);
        }
        catch(const std::exception& er_db)
        {
                log_error(er_db.what());
        }
        return 0;
}

unsigned int getContextIdByName(const std::string& _name, dbconn& conn)
{
        try
        {
                tntdb::Statement q_ctx_id = conn.prepare(
                        "SELECT id FROM context WHERE name = :Qname");
                q_ctx_id.setString("Qname", _name);
                tntdb::Row row_ctx_id = q_ctx_id.selectRow();
                if(!row_ctx_id.empty() && !row_ctx_id[0].isNull())
			return row_ctx_id[0].getUnsigned();
        }
        catch(const std::exception& er_ctx)
        {
                log_warn(er_ctx.what());
        }
        return 0;
}

} //namespace vagra
