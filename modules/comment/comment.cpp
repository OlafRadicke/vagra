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
#include <tntdb/result.h>
#include <tntdb/transaction.h>
#include <tntdb/row.h>

#include <vagra/nexus.h>

#include <vagra/comment/comment.h>
#include <vagra/comment/cachedcomment.h>
#include <vagra/article/cachedarticle.h>

namespace vagra
{

log_define("vagra")

//begin Comment

Comment::Comment(const unsigned int comm_id)
{
	id = comm_id;
	try
	{
		Nexus& nx = Nexus::getInstance();
		dbconn conn = nx.dbConnection();
		tntdb::Statement q_comment = conn.prepare(
			"SELECT art_id, comm_ref, comment, name, home, mail, ctime, mtime"
				" FROM comments WHERE comm_id = :Qid");
		q_comment.setUnsigned("Qid", id);
		tntdb::Row row_comment = q_comment.selectRow();
		if(!row_comment[0].isNull())
			art_id = row_comment[0].getUnsigned();
		if(!row_comment[1].isNull())
			ref_id = row_comment[1].getUnsigned();
		if(!row_comment[2].isNull())
			text = row_comment[2].getString();
		if(!row_comment[3].isNull())
			author = row_comment[3].getString();
		if(!row_comment[4].isNull())
			homepage = row_comment[4].getString();
		if(!row_comment[5].isNull())
			mail = row_comment[5].getString();
		if(!row_comment[6].isNull())
			ctime = row_comment[6].getDatetime();
		if(!row_comment[7].isNull())
			mtime = row_comment[7].getDatetime();
	}
	catch(const std::exception& er_comm)
	{
		log_error(er_comm.what());
	}
}

Comment::operator bool() const
{
	return(art_id && !text.empty());
}

void Comment::clear()
{
	clearBase();
	ref_id = 0;
	art_id = 0;
	homepage.clear();
	mail.clear();
}

const unsigned int Comment::getArticle() const
{
	return art_id;
}

const unsigned int Comment::getRef() const
{
	return ref_id;
}

const std::string& Comment::getHomepage() const
{
	return homepage;
}

const std::string& Comment::getMail() const
{
	return mail;
}

void Comment::setArticle(const unsigned int article_id)
{
	art_id = article_id;
}

void Comment::setRef(const unsigned int referece_id)
{
	ref_id = referece_id;
}

void Comment::setHomepage(const std::string& s)
{
	homepage = s;
}

void Comment::setMail(const std::string& s)
{
	mail = s;
}

void Comment::dbInsert()
{
	if(author.empty())
		throw std::domain_error(gettext("need an author"));
	if(text.empty())
		throw std::domain_error(gettext("need text"));
	if(!art_id)
		throw std::domain_error(gettext("need article id"));

	Nexus& nx = Nexus::getInstance();
	dbconn conn = nx.dbConnection();

	tntdb::Transaction trans_comm(conn);
	try
	{
		conn.prepare("INSERT INTO comments (art_id, comm_ref, comment , name, home, mail)"
			       " VALUES (:Iart_id, :Iref_id, :Itext, :Iauthor, :Ihomepage, :Imail)")
		.setUnsigned("Iart_id", art_id)
		.setUnsigned("Iref_id", ref_id)
		.setString("Itext", text)
		.setString("Iauthor", author)
		.setString("Ihomepage", homepage)
		.setString("Imail", mail)
		.execute();
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not insert comment"));
	}

	try
	{
		ArticleCache& art_cache = ArticleCache::getInstance();
		trans_comm.commit();
		art_cache.clear(art_id);
	}
	catch(const std::exception& er_trans)
	{
		log_error(er_trans.what());
		throw std::domain_error(gettext("commit failed"));
	}
}

void Comment::dbUpdate()
{
	if(author.empty())
		throw std::domain_error(gettext("need an author"));
	if(text.empty())
		throw std::domain_error(gettext("need text"));
	if(!art_id)
		throw std::domain_error(gettext("need valid article id"));

	Nexus& nx = Nexus::getInstance();
	dbconn conn = nx.dbConnection();

	tntdb::Transaction trans_comm(conn);
	try
	{
		conn.prepare("UPDATE comments SET comm_ref = :Iref_id, comment = :Itext, name = :Iauthor, "
				"home = :Ihomepage, mail = :Imail, mtime = now() WHERE comm_id = :Icomm_id")
		.setUnsigned("Iref_id", ref_id)
		.setString("Itext", text)
		.setString("Iauthor", author)
		.setString("Ihomepage", homepage)
		.setString("Imail", mail)
		.setUnsigned("Icomm_id", id)
		.execute();
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not update article"));
	}

	try
	{
		Cache<Comment>& comm_cache = Cache<Comment>::getInstance();
		trans_comm.commit();
		comm_cache.clear(id);
	}
	catch(const std::exception& er_trans)
	{
		log_error(er_trans.what());
		throw std::domain_error(gettext("commit failed"));
	}
}
//end Comment

} //namespace vagra
