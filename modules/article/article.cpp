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
#include <algorithm>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>
#include <tntdb/result.h>
#include <tntdb/transaction.h>
#include <tntdb/row.h>

#include <vagra/helpers.h>
#include <vagra/nexus.h>

#include <vagra/article/article.h>
#include <vagra/article/articlecache.h>

namespace vagra
{

log_define("vagra")

//begin Article

Article::Article(const std::string& art_title)
{
	*this = Article(cachedGetArticleIdByTitle(art_title));
}

Article::Article(const unsigned int art_id)
{
	id = art_id;
	try
	{
		Nexus& nx = Nexus::getInstance();
		dbconn conn = nx.dbConnection();
  		try
		{
			tntdb::Statement q_art_cont = conn.prepare(
				"SELECT title, headline, abstract, content, author, ctime, mtime"
					" FROM articles WHERE art_id = :Qid");
			q_art_cont.setUnsigned("Qid", art_id);
			tntdb::Row row_art_cont = q_art_cont.selectRow();
			if(!row_art_cont[0].isNull())
				title = row_art_cont[0].getString();
			if(!row_art_cont[1].isNull())
				head = row_art_cont[1].getString();
			if(!row_art_cont[2].isNull())
				abstract = row_art_cont[2].getString();
			if(!row_art_cont[3].isNull())
				text = row_art_cont[3].getString();
			if(!row_art_cont[4].isNull())
				author = row_art_cont[4].getString();
			if(!row_art_cont[5].isNull())
				ctime = row_art_cont[5].getDatetime();
			if(!row_art_cont[6].isNull())
				mtime = row_art_cont[6].getDatetime();
			url = space2underscore(title);
		}
		catch(const std::exception& er_art)
		{
			log_error(er_art.what());
    		}
		try
		{
			tntdb::Statement q_art_tags = conn.prepare(
				"SELECT tag FROM tags WHERE art_id = :Qid");
			q_art_tags.setUnsigned("Qid", art_id);
			tntdb::Result res_art_tags = q_art_tags.select();
      			for(tntdb::Result::const_iterator it = res_art_tags.begin();
				it != res_art_tags.end(); ++it)
			{
				tntdb::Row row_art_tags = *it;
				if(row_art_tags[0].isNull())
					throw std::domain_error(gettext("got an NULL tag"));
				tags.push_back(row_art_tags[0].getString());
      			}
    		}
		catch(const std::exception& er_tag)
		{
			log_error(er_tag.what());
		}
		try
		{
			tntdb::Statement q_art_comm_ids = conn.prepare(
				"SELECT comm_id FROM comments WHERE art_id = :Qid ORDER BY comm_id ASC");
			q_art_comm_ids.setUnsigned("Qid", art_id);
			tntdb::Result res_art_comm_ids = q_art_comm_ids.select();
      			for(tntdb::Result::const_iterator it = res_art_comm_ids.begin();
				it != res_art_comm_ids.end(); ++it)
			{
				tntdb::Row row_art_comm_id = *it;
				if(row_art_comm_id[0].isNull())
					throw std::domain_error(gettext("got an NULL comment"));
				comment_ids.push_back(row_art_comm_id[0].getUnsigned());
      			}
		}
		catch(const std::exception& er_comm)
		{
			log_error(er_comm.what());
		}
  	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
  	}
}

void Article::clear()
{
	clearBase();
	url.clear();
	tags.clear();
	comment_ids.clear();
}

const std::string& Article::getUrl() const
{
	return url;
}

const std::vector<std::string>& Article::getTags() const
{
	return tags;
}

std::string Article::getTagString() const
{
	std::string tag_str;
	if(!tags.empty())
	{
		for(std::vector<std::string>::const_iterator it = tags.begin();
				it != tags.end(); it++)
		{
			if(it != tags.begin())
				tag_str += ", ";
			tag_str += *it;
		}
	}
	return tag_str;
}

std::vector<unsigned int> Article::getCommentIds() const
{
	return comment_ids;
}

const szt_vecint Article::getCommentAmount() const
{
	return comment_ids.size();
}

void Article::setTags(const std::string& s)
{
	tags.clear();
	if(!s.empty())
	{
		size_t this_pos=0;
		size_t last_pos=0;
		do
		{
			this_pos = s.find(",", last_pos);
			if(this_pos == std::string::npos)
				this_pos=s.size();
			std::string ts(s.substr(last_pos,(this_pos - last_pos)));
			ts.erase(std::remove_if(ts.begin(), ts.end(), invalidChar()), ts.end());
			if(!ts.empty() && !std::count(tags.begin(), tags.end(), ts))
			{
				tags.push_back(ts);
			}
			last_pos = this_pos + 1;
		} while(last_pos <= s.size());
	}

}

void Article::dbInsert()
{
	title = underscore2space(title);

	if(title.empty())
		throw std::domain_error(gettext("title empty"));
	if(head.empty())
		throw std::domain_error(gettext("headline empty"));
	if(text.empty() && abstract.empty())
		throw std::domain_error(gettext("need abstract or text"));

	Nexus& nx = Nexus::getInstance();
	dbconn conn = nx.dbConnection();

	if(getArticleIdByTitle(title, conn))
		throw std::domain_error(gettext("article already exist"));

	tntdb::Transaction trans_art(conn);
	try
	{
		conn.prepare("INSERT INTO articles (title, headline, abstract, content, author)"
			       " VALUES (:Ititle, :Ihead, :Iabstract, :Itext, :Iauthor)")
		.setString("Ititle", title)
		.setString("Ihead", head)
		.setString("Iabstract", abstract)
		.setString("Itext", text)
		.setString("Iauthor", author)
		.execute();
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not insert article"));
	}

	unsigned int art_id = getArticleIdByTitle(title, conn);
	if(!art_id)
		throw std::domain_error(gettext("article id not found"));
	id = art_id;
	try
	{
		for(std::vector<std::string>::iterator it = tags.begin();
				it != tags.end(); it++)
		{
			conn.prepare("INSERT INTO tags (tag, art_id)"
					" VALUES (:Itag, :Iid)")
			.setString("Itag", *it)
			.setUnsigned("Iid", art_id)
			.execute();
		}
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not commit tags"));
	}
	try
	{
		ArticleCache& art_cache = ArticleCache::getInstance();
		trans_art.commit();
		art_cache.updateTagsum();
		art_cache.invAdd(art_id);
		art_cache.updateMTime();
	}
	catch(const std::exception& er_trans)
	{
		log_error(er_trans.what());
		throw std::domain_error(gettext("commit failed"));
	}
}

void Article::dbUpdate()
{
	title = underscore2space(title);

	if(title.empty())
		throw std::domain_error(gettext("title empty"));
	if(head.empty())
		throw std::domain_error(gettext("headline empty"));
	if(text.empty() && abstract.empty())
		throw std::domain_error(gettext("need abstract or text"));

	Nexus& nx = Nexus::getInstance();
	dbconn conn = nx.dbConnection();

	tntdb::Transaction trans_art(conn);
	try
	{
		conn.prepare("UPDATE articles SET headline = :Ihead, abstract = :Iabstract, "
			     "content = :Itext, author = :Iauthor, mtime = now() WHERE art_id = :Iid")
		.setString("Ihead", head)
		.setString("Iabstract", abstract)
		.setString("Itext", text)
		.setString("Iauthor", author)
		.setUnsigned("Iid", id)
		.execute();
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not update article"));
	}

	try
	{
		conn.prepare("DELETE FROM tags WHERE art_id = :Iid")
			.setUnsigned("Iid", id)
			.execute();

		for(std::vector<std::string>::iterator it = tags.begin();
				it != tags.end(); it++)
		{
			conn.prepare("INSERT INTO tags (tag, art_id)"
					" VALUES (:Itag, :Iid)")
			.setString("Itag", *it)
			.setUnsigned("Iid", id)
			.execute();
		}
	}
	catch(const std::exception& er_db)
	{
		log_error(er_db.what());
		throw std::domain_error(gettext("could not commit tags"));
	}
	try
	{
		ArticleCache& art_cache = ArticleCache::getInstance();
		trans_art.commit();
		art_cache.clear(id);
		art_cache.updateTagsum();
		art_cache.updateMTime();
	}
	catch(const std::exception& er_trans)
	{
		log_error(er_trans.what());
		throw std::domain_error(gettext("commit failed"));
	}
}

//end Article

unsigned int getArticleIdByTitle(const std::string& title)
{
        try
        {
		Nexus& nx = Nexus::getInstance();
		dbconn conn = nx.dbConnection();
		return getArticleIdByTitle(title, conn);
	}
	catch(const std::exception& er_comm)
	{
		log_error(er_comm.what());
	}
	return 0;
}

/* need this versions of getArticleIdByTitle, because the art_id must be visible 
 * during transaction in Article::dbInsert().
 */
unsigned int getArticleIdByTitle(const std::string& title, dbconn& conn)
{
        std::string art_title(underscore2space(title));
        try
        {
                tntdb::Statement q_art_id = conn.prepare(
                        "SELECT art_id FROM articles WHERE title = :Qtitle");
                q_art_id.setString("Qtitle", art_title);
                tntdb::Row row_art_id = q_art_id.selectRow();
                if(row_art_id[0].isNull())
                        throw std::domain_error(gettext("title has no ID but have to"));
                return row_art_id[0].getUnsigned();
        }
        catch(const std::exception& er_comm)
        {
                log_warn(er_comm.what());
        }
        return 0;
}

} //namespace vagra
