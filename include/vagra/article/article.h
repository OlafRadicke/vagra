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

#ifndef VARGA_ARTICLE_H
#define VARGA_ARTICLE_H

#include <string>
#include <vector>

#include <vagra/types.h>
#include <vagra/baseobject.h>

namespace vagra
{

class Article: public BaseObject
{
	std::string title;
	std::string head;
	std::string abstract;
	std::string text;
	std::string author;
	std::string url;
	bool comments_allow;
	bool comments_view;
	std::vector<std::string> tags;
	std::vector<unsigned int> comment_ids;

    public:
	Article() : BaseObject("articles"),
		comments_allow(false), comments_view(false) {}
	explicit Article(const std::string&, const BaseAuth& = BaseAuth());
	explicit Article(const unsigned int, const BaseAuth& = BaseAuth());
	operator bool() const;
	Article* operator->() { return this; }
	void clear();

	const std::string& getTitle() const;
	const std::string& getHead() const;
	const std::string& getAbstract() const;
	const std::string& getText() const;
	const std::string& getAuthor() const;
	const std::string& getUrl() const;
	const bool getCommentsAllow() const;
	const bool getCommentsView() const;
	const std::vector<std::string>& getTags() const;
	std::string getTagString() const;
	std::vector<unsigned int> getCommentIds() const;
	const szt_vecint getCommentAmount() const;

	void setTitle(const std::string&);
	void setHead(const std::string&);
	void setAbstract(const std::string&);
	void setText(const std::string&);
	void setTags(const std::string&);
	void setCommentsAllow(bool);
	void setCommentsView(bool);

	void dbCommit(const BaseAuth& = BaseAuth());

	static unsigned int getIdByName(const std::string&);
};

unsigned int getArticleIdByTitle (const std::string&);
unsigned int getArticleIdByTitle (const std::string&, dbconn&);

} //namespace vagra

#endif // VAGRA_ARTICLE_H
