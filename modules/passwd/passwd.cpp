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

#include <stdlib.h> // remove me when radom strings
#include <time.h>   // are available in cxxtools

#include <stdexcept>
#include <libintl.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <cxxtools/md5.h>
#include <cxxtools/hmac.h>
#include <tntdb/result.h>
#include <tntdb/transaction.h>
#include <tntdb/row.h>

#include <vagra/nexus.h>

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
			throw std::domain_error(gettext("unknown passwd id"));
		if(!row_pw[0].isNull())
			salt = row_pw[0].getString();
		if(!row_pw[1].isNull())
			hash = row_pw[1].getString();
		id = _id;
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not read passwd"));
	}
}

Passwd::Passwd(const std::string& passwd, const unsigned int _aid)
	: BaseObject("vpasswd", _aid)
{
	genHash(passwd);
}

void Passwd::Passwd::clear()
{
	clearBase();

	salt.clear();
	hash.clear();
}

void Passwd::genHash(const std::string& passwd)
{
	if(passwd.empty())
		throw std::domain_error(gettext("empty passwords are not allowed"));

	try
	{
		std::string _randstr; // remove me when radom strings
		_randstr.reserve(64); // are available in cxxtools
		srand48(time(NULL));

		for(int n = 0; n <= 64; n++)
  		{
			char c(mrand48()%127);
			if(c < 0)
				c = ~c;
			_randstr.push_back(c);
		}
		salt = cxxtools::hmac<cxxtools::md5_hash<std::string> >(_randstr, passwd);
		hash = cxxtools::hmac<cxxtools::md5_hash<std::string> >(salt, passwd);
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
		throw std::domain_error(gettext("empty passwords are not allowed"));
	return hash == cxxtools::hmac<cxxtools::md5_hash<std::string> >(salt, _passwd);
}


void Passwd::dbCommit(const unsigned int _aid)
{
	if(!id)
		throw std::domain_error(gettext("need valid user id"));
	if(hash.empty() || salt.empty())
		throw std::domain_error(gettext("need valid password hash"));
	
	Nexus& nx = Nexus::getInstance();
	dbconn conn = nx.dbConnection();

	tntdb::Transaction trans_passwd(conn);
	try
	{
		dbCommitBase(conn, _aid); //init base, INSERT if id==0, otherwise UPDATE

		conn.prepare("UPDATE vpasswd SET salt = :Isalt, hash = :Ihash"
				" WHERE id = :Iid")
			.setString("Isalt",salt)
			.setString("Ihash", hash)
			.setUnsigned("Iid", id)
			.execute();
		trans_passwd.commit();
	}
	catch(const std::exception& er_trans)
	{
		log_error(er_trans.what());
		throw std::domain_error(gettext("could not commit passwd"));
	}
}

// end Passwd

} //namespace vagra
