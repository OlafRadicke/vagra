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

#ifndef VARGA_COMMENT_H
#define VARGA_COMMENT_H

#include <string>

#include <vagra/types.h>
#include <vagra/baseobject.h>

namespace vagra
{

class Comment: public BaseObject
{
	unsigned int ref_id;
	unsigned int art_id;
	std::string text;
	std::string author;
	std::string homepage;
	std::string mail;

    public:
	Comment() : BaseObject("comments"),
		ref_id(0),
		art_id(0) {}
	explicit Comment(const unsigned int, const unsigned int = 0);
	operator bool() const;
	Comment* operator->() { return this; }
	void clear();

	const unsigned int getRef() const;
	const unsigned int getArticle() const;
	const std::string& getText() const;
	const std::string& getAuthor() const;
	const std::string& getHomepage() const;
	const std::string& getMail() const;

	void setRef(const unsigned int);
	void setArticle(const unsigned int);
	void setText(const std::string&);
	void setAuthor(const std::string&);
	void setHomepage(const std::string&);
	void setMail(const std::string&);

	void dbCommit(const unsigned int = 0);
};

} //namespace vagra

#endif // VAGRA_COMMENT_H
