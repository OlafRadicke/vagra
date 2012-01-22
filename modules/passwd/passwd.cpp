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

Passwd::Passwd(const unsigned int _uid)
{
	try
	{
		Nexus& nx = Nexus::getInstance();
		dbconn conn = nx.dbConnection();

		tntdb::Statement q_uid = conn.prepare(
			"SELECT salt, hash FROM vpasswd WHERE uid = :Quid");
		q_uid.setUnsigned("Quid", _uid);
		tntdb::Row row_uid = q_uid.selectRow();
		if(row_uid.empty())
			throw std::domain_error(gettext("unknown uid"));
		if(!row_uid[0].isNull())
			salt = row_uid[0].getString();
		if(!row_uid[1].isNull())
			hash = row_uid[1].getString();
		uid = _uid;
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not read passwd"));
	}
}

Passwd::Passwd(const std::string& passwd)
	: uid(0)
{
	genHash(passwd);
}

Passwd::Passwd(const unsigned int _uid, const std::string& passwd)
	: uid(_uid)
{
	genHash(passwd);
}

void Passwd::setUser(const unsigned int _uid)
{
	uid = _uid;
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

bool Passwd::verify(const std::string& passwd)
{
	if(passwd.empty())
		throw std::domain_error(gettext("empty passwords are not allowed"));
	return hash == cxxtools::hmac<cxxtools::md5_hash<std::string> >(salt, passwd);
}


void Passwd::dbUpdate()
{
	if(!uid)
		throw std::domain_error(gettext("need valid user id"));
	if(hash.empty() || salt.empty())
		throw std::domain_error(gettext("need valid password hash"));
	
	Nexus& nx = Nexus::getInstance();
	dbconn conn = nx.dbConnection();

	tntdb::Transaction trans_passwd(conn);
	try
	{
		tntdb::Statement q_uid = conn.prepare(
			"SELECT uid FROM vpasswd WHERE uid = :Quid");
		q_uid.setUnsigned("Quid", uid);
		tntdb::Result res_uid = q_uid.select();
		if(res_uid.empty())
		{
			conn.prepare("INSERT INTO vpasswd (salt, hash, uid) VALUES"
					" (:Isalt, :Ihash, :Iuid)")
				.setString("Isalt", salt)
				.setString("Ihash", hash)
				.setUnsigned("Iuid", uid)
				.execute();
		}
		else
		{
			conn.prepare("UPDATE vpasswd SET salt = :Isalt, hash = :Ihash"
					" WHERE uid = :Iuid")
				.setString("Isalt",salt)
				.setString("Ihash", hash)
				.setUnsigned("Iuid", uid)
				.execute();
		}
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
