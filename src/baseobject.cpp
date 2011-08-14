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

#include <vagra/baseobject.h>

namespace vagra
{

//begin BaseObject

BaseObject::operator bool() const
{
	return(!title.empty() && !head.empty() && (!text.empty() || !abstract.empty()));
}

void BaseObject::clearBase()
{
	id = 0;
	title.clear();
	head.clear();
	abstract.clear();
	text.clear();
	author.clear();
	ctime = vdate();
	mtime = vdate();
}

const unsigned int BaseObject::getId() const
{
	return id;
}

const std::string& BaseObject::getTitle() const
{
	return title;
}

const std::string& BaseObject::getHead() const
{
	return head;
}

const std::string& BaseObject::getAbstract() const
{
	return abstract;
}

const std::string& BaseObject::getText() const
{
	return text;
}

const std::string& BaseObject::getAuthor() const
{
	return author;
}

const vdate& BaseObject::getCTime() const
{
	return ctime;
}

const vdate& BaseObject::getMTime() const
{
	return mtime;
}

void BaseObject::setTitle(const std::string& s)
{
	title = s;
}

void BaseObject::setHead(const std::string& s)
{
	head = s;
}

void BaseObject::setAbstract(const std::string& s)
{
	abstract = s;
}

void BaseObject::setText(const std::string& s)
{
	text = s;
}

void BaseObject::setAuthor(const std::string& s)
{
	author = s;
}

//end BaseObject

} //namespace vagra
