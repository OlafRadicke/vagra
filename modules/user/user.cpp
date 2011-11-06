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

#include <stdexcept>
#include <libintl.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <cxxtools/regex.h>
#include <tntdb/result.h>
#include <tntdb/transaction.h>
#include <tntdb/row.h>

#include <vagra/nexus.h>
#include <vagra/cache.h>

#include <vagra/user/user.h>

namespace vagra
{

log_define("vagra")

//begin User

User::User(const unsigned int u_id)
{
	id = u_id;
	try
	{
		Nexus& nx = Nexus::getInstance();
		dbconn conn = nx.dbConnection();
		tntdb::Statement q_user = conn.prepare(
			"SELECT logname, dispname, surname, name, mail,"
				" ctime, mtime FROM user WHERE u_id = :Qid");
		q_user.setUnsigned("Qid", id);
		tntdb::Row row_user = q_user.selectRow();
		if(!row_user[0].isNull())
			logname = row_user[0].getString();
		if(!row_user[1].isNull())
			dispname = row_user[1].getString();
		if(!row_user[2].isNull())
			surname = row_user[2].getString();
		if(!row_user[3].isNull())
			name = row_user[3].getString();
		if(!row_user[4].isNull())
			mail = row_user[4].getString();
		if(!row_user[5].isNull())
			ctime = row_user[5].getDatetime();
		if(!row_user[6].isNull())
			mtime = row_user[6].getDatetime();
	}
	catch(const std::exception& er_user)
	{
		log_error(er_user.what());
	}
}

User::operator bool() const
{
	return(id && !logname.empty());
}

void User::clear()
{
	clearBase();
	logname.clear();
	dispname.clear();
	surname.clear();
	name.clear();
	mail.clear();
}

const std::string& User::getLogname() const
{
	return logname;
}

const std::string& User::getDispname() const
{
	return dispname;
}

const std::string& User::getSurname() const
{
	return surname;
}

const std::string& User::getName() const
{
	return name;
}

const std::string& User::getMail() const
{
	return mail;
}

void User::setLogname(const std::string& s)
{
	logname = s;
}

void User::setDispname(const std::string& s)
{
	dispname = s;
}

void User::setSurname(const std::string& s)
{
	surname = s;
}

void User::setName(const std::string& s)
{
	name = s;
}

void User::setMail(const std::string& s)
{
	mail = s;
}

void User::dbInsert()
{
	if(logname.empty())
		throw std::domain_error(gettext("need a login name"));
	if(logname.length() > 16)
		throw std::domain_error(gettext("logname to long"));

	cxxtools::Regex checkLogname("^[aA-zZ]*$");
	if(!checkLogname.match(logname))
		throw std::domain_error(gettext("invalid logname"));

	Nexus& nx = Nexus::getInstance();
	dbconn conn = nx.dbConnection();

	if(getUidByLogname(logname,conn))
		throw std::domain_error(gettext("logname belongs to differen user"));

	tntdb::Transaction trans_user(conn);
	try
	{
		conn.prepare("INSERT INTO user (logname, dispname, surname,"
			" name, mail) VALUES (:Ilogname, :Idispname, :Isurname,"
			" :Iname, :Imail)")
		.setString("Ilogname", logname)
		.setString("Idispname", dispname)
		.setString("Isurname", surname)
		.setString("Iname", name)
		.setString("Imail", mail)
		.execute();
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not insert user"));
	}

	try
	{
		trans_user.commit();
	}
	catch(const std::exception& er_trans)
	{
		log_error(er_trans.what());
		throw std::domain_error(gettext("commit failed"));
	}
}

void User::dbUpdate()
{
	if(!id)
		throw std::domain_error(gettext("need valid user id"));
	if(logname.empty())
		throw std::domain_error(gettext("need a login name name"));
	if(logname.length() > 16)
		throw std::domain_error(gettext("logname to long"));
	
	cxxtools::Regex checkLogname("^[aA-zZ]*$");
	if(!checkLogname.match(logname))
		throw std::domain_error(gettext("invalid logname"));

	Nexus& nx = Nexus::getInstance();
	dbconn conn = nx.dbConnection();

	unsigned int _id = getUidByLogname(logname,conn);
	if(_id && _id != id)
		throw std::domain_error(gettext("logname belongs to differen user"));

	tntdb::Transaction trans_user(conn);
	try
	{
		conn.prepare("UPDATE user SET logname = :Ilogname, dispname = :Idispname,"
				" surname = :Isurname, name = :Iname, mail = :Imail,"
			        " mtime = now() WHERE id = :Iu_id")
		.setString("Ilogname", logname)
		.setString("Idispname", dispname)
		.setString("Isurname", surname)
		.setString("Iname", name)
		.setString("Imail", mail)
		.setUnsigned("Iu_id", id)
		.execute();
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not update user"));
	}

	try
	{
		Cache<User>& user_cache = Cache<User>::getInstance();
		trans_user.commit();
		user_cache.clear(id);
	}
	catch(const std::exception& er_trans)
	{
		log_error(er_trans.what());
		throw std::domain_error(gettext("commit failed"));
	}
}

// end User

unsigned int getUidByLogname(const std::string& logname)
{
        try
        {
                Nexus& nx = Nexus::getInstance();
                dbconn conn = nx.dbConnection();
                return getUidByLogname(logname, conn);
        }
        catch(const std::exception& er_comm)
        {
                log_error(er_comm.what());
        }
        return 0;
}

/* need this versions , because we may
 * read id during transaction.
 */
unsigned int getUidByLogname(const std::string& logname, dbconn& conn)
{
        try
        {
                tntdb::Statement q_u_id = conn.prepare(
                        "SELECT id FROM user WHERE logname = :Qlogname");
                q_u_id.setString("Qlogname", logname);
                tntdb::Row row_u_id = q_u_id.selectRow();
                if(!row_u_id[0].isNull())
                	return row_u_id[0].getUnsigned();
        }
        catch(const std::exception& er_uid)
        {
                log_warn(er_uid.what());
        }
        return 0;
}

} //namespace vagra