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
#include <cxxtools/log.h>
#include <cxxtools/md5.h>
#include <cxxtools/hmac.h>
#include <tntdb/result.h>
#include <tntdb/transaction.h>
#include <tntdb/row.h>

#include <vagra/exception.h>
#include <vagra/nexus.h>
#include <vagra/cache.h>
#include <vagra/utils.h>

#include <vagra/passwd/passwd.h>

namespace vagra
{

log_define("vagra")

//begin Passwd

Passwd::Passwd(const unsigned int _id, const unsigned int _aid)
	: BaseObject("vpasswd", _id, _aid) //call baseconstructor(db_tablename,id,authId)
{
	try
	{
		Nexus& nx = Nexus::getInstance();
		dbconn conn = nx.dbConnection();

		tntdb::Statement q_pw = conn.prepare(
			"SELECT salt, hash FROM vpasswd WHERE id = :Qid");
		q_pw.setUnsigned("Qid", _id);
		tntdb::Row row_pw = q_pw.selectRow();
		if(row_pw.empty())
			throw InvalidObject(gettext("unknown passwd id"));
		if(!row_pw[0].isNull())
			salt = row_pw[0].getString();
		if(!row_pw[1].isNull())
			hash = row_pw[1].getString();
		id = _id;
	}
	catch(const Exception&)
	{
		throw;
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not read passwd"));
	}
}

Passwd::Passwd(const std::string& _passwd, const unsigned int _aid)
	: BaseObject("vpasswd", _aid)
{
	genHash(_passwd);
}

Passwd::operator bool() const
{
	return (!salt.empty() && !hash.empty());
}


void Passwd::Passwd::clear()
{
	clearBase();

	salt.clear();
	hash.clear();
}

void Passwd::genHash(const std::string& _passwd)
{
	if(_passwd.empty())
		throw InvalidValue(gettext("empty passwords are not allowed"));

	try
	{
		vagra::randomString randstr(64);
		salt = cxxtools::hmac<cxxtools::md5_hash<std::string> >(randstr, _passwd);
		hash = cxxtools::hmac<cxxtools::md5_hash<std::string> >(salt, _passwd);
	}
	catch(const std::exception& er_hash)
	{
		log_error(er_hash.what());
		throw std::domain_error(gettext("could not generate hash"));
	}
}

bool Passwd::verify(const std::string& _passwd) const
{
	if(_passwd.empty())
		throw InvalidValue(gettext("empty passwords are not allowed"));
	return hash == cxxtools::hmac<cxxtools::md5_hash<std::string> >(salt, _passwd);
}

void Passwd::update(const std::string& _passwd)
{
	genHash(_passwd);
}

void Passwd::dbCommit(const unsigned int _aid)
{
	try{
		Nexus& nx = Nexus::getInstance();
		dbconn conn = nx.dbConnection();

		tntdb::Transaction trans_passwd(conn);
		dbCommit(conn, _aid);
		Cache<Passwd>& passwd_cache = Cache<Passwd>::getInstance();
		trans_passwd.commit();
		passwd_cache.clear(id);
	}
	catch(const Exception&)
	{
		throw;
	}
	catch(const std::exception& er_trans)
	{
		log_error(er_trans.what());
		throw std::domain_error(gettext("could not commit passwd"));
	}
}

void Passwd::dbCommit(dbconn& conn, const unsigned int _aid)
{
	if(hash.empty() || salt.empty())
		throw InvalidValue(gettext("need valid password hash"));
	
	try
	{
		dbCommitBase(conn, _aid); //init base, INSERT if id==0, otherwise UPDATE

		conn.prepare("UPDATE vpasswd SET salt = :Isalt, hash = :Ihash"
				" WHERE id = :Iid")
			.setString("Isalt",salt)
			.setString("Ihash", hash)
			.setUnsigned("Iid", id)
			.execute();
	}
	catch(const Exception&)
	{
		throw;
	}
	catch(const std::exception& er_trans)
	{
		log_error(er_trans.what());
		throw std::domain_error(gettext("could not update passwd"));
	}
}

// end Passwd

} //namespace vagra
