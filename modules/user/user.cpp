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
#include <cxxtools/regex.h>
#include <tntdb/result.h>
#include <tntdb/transaction.h>
#include <tntdb/row.h>

#include <vagra/exception.h>
#include <vagra/nexus.h>
#include <vagra/cache.h>
#include <vagra/utils.h>

#include <vagra/user/user.h>

namespace vagra
{

log_define("vagra")

//begin User

User::User(const unsigned int _id, const BaseAuth& _auth)
	: BaseObject("vuser", _id, _auth) //call baseconstructor(db_tablename,id,authId)
{
	Init(_auth);
}

User::User(const std::string& _name, const BaseAuth& _auth)
	: BaseObject("vuser", getIdByName(_name), _auth)
{
	Init(_auth);
}

void User::Init(const BaseAuth& _auth)
{
	try
	{
		Nexus& nx = Nexus::getInstance();
		dbconn conn = nx.dbConnection();
		tntdb::Statement q_user = conn.prepare(
			"SELECT logname, dispname, pw_id FROM vuser WHERE id = :Qid");
		q_user.setUnsigned("Qid", id);
		tntdb::Row row_user = q_user.selectRow();
		if(!row_user[0].isNull())
			logname = row_user[0].getString();
		if(!row_user[1].isNull())
			dispname = row_user[1].getString();
		if(!row_user[2].isNull() && row_user[2].getUnsigned())
			password = Passwd(row_user[2].getUnsigned(), _auth);
	}
	catch(const Exception&)
	{
		log_error(gettext("cannot read passwd"));
		throw;
	}
	catch(const std::exception& er_user)
	{
		log_error(er_user.what());
		throw std::domain_error(gettext("failed to read user data"));
	}
}

User::operator bool() const
{
	return(id && !logname.empty());
}

User::operator int() const
{
	return(id);
}

User::operator unsigned int() const
{
	return(id);
}

void User::clear()
{
	clearBase();

	logname.clear();
	dispname.clear();
	password.clear();
}

const std::string& User::getLogname() const
{
	return logname;
}

const std::string& User::getDispname() const
{
	return dispname;
}

const Passwd& User::getPasswd() const
{
	return password;
}

void User::setLogname(const std::string& s)
{
	logname = s;
}

void User::setDispname(const std::string& s)
{
	dispname = s;
}

void User::setPasswd(const Passwd& _pw)
{
	if(password)
		throw InvalidValue(gettext("user already has a password"));
	if(!_pw)
		throw InvalidValue(gettext("invalid password submitted"));
	if(_pw.getOwner() != id)
		throw InvalidValue(gettext("password must be owned by the user"));
	password = _pw;
}

void User::setPasswd(const std::string& _pw)
{
	if(_pw.empty())
		throw InvalidValue(gettext("empty password submitted"));
	password.update(_pw);
}

std::string User::setRandomPasswd(const BaseAuth& _auth)
{
	password.setContext("passwd", _auth);
	password.setOwner(id, _auth);
	vagra::randomString randstr(12);
	password.update(randstr);
	return randstr;
}

void User::dbCommit(const BaseAuth& _auth)
{
	if(logname.empty())
		throw InvalidValue(gettext("need a login name name"));
	if(logname.length() > 16)
		throw InvalidValue(gettext("logname to long"));
	
	cxxtools::Regex checkLogname("^[aA-zZ]*$");
	if(!checkLogname.match(logname))
		throw InvalidValue(gettext("invalid logname"));

	if(!password)
		throw InvalidValue(gettext("password not set"));

	Nexus& nx = Nexus::getInstance();
	dbconn conn = nx.dbConnection();

	unsigned int _id = getUidByLogname(logname,conn);
	if(_id && _id != id)
		throw InvalidValue(gettext("logname belongs to differen user"));

	if(dispname.empty())
		dispname = logname;

	_id = getUidByDispname(dispname,conn);
	if(_id && _id != id)
		throw InvalidValue(gettext("dispname belongs to differen user"));

	_id = getUidByLogname(dispname,conn); // do not allow to set other users logname as dispname
	if(_id && _id != id)
		throw InvalidValue(gettext("dispname belongs to differen user"));

	tntdb::Transaction trans_user(conn);
	try
	{
		dbCommitBase(conn, _auth); //init base, INSERT if id==0, otherwise UPDATE
		if(!password.getOwner())
			password.setOwner(id, _auth);
		password.dbCommit(conn, _auth);

		conn.prepare("UPDATE vuser SET logname = :Ilogname, dispname = :Idispname,"
			" pw_id = :Ipw_id WHERE id = :Iid")
		.setString("Ilogname", logname)
		.setString("Idispname", dispname)
		.setUnsigned("Ipw_id", password.getId())
		.setUnsigned("Iid", id)
		.execute();
	}
	catch(const Exception&)
	{
		throw;
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not write user data"));
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

void User::passwdCommit(const BaseAuth& _auth)
{
	Cache<User>& user_cache = Cache<User>::getInstance();
	password.dbCommit(_auth);
	user_cache.clear(id);
}

unsigned int User::getIdByName(const std::string& _name)
{
	unsigned int _uid = getUidByLogname(_name);
	if(!_uid)
		throw InvalidObject(gettext("unknown user"));

	return _uid;
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

unsigned int getUidByDispname(const std::string& dispname)
{
        try
        {
                Nexus& nx = Nexus::getInstance();
                dbconn conn = nx.dbConnection();
                return getUidByDispname(dispname, conn);
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
                        "SELECT id FROM vuser WHERE logname = :Qlogname");
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

unsigned int getUidByDispname(const std::string& dispname, dbconn& conn)
{
        try
        {
                tntdb::Statement q_u_id = conn.prepare(
                        "SELECT id FROM vuser WHERE dispname = :Qdispname");
                q_u_id.setString("Qdispname", dispname);
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
