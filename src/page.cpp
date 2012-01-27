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

#include <vagra/nexus.h>
#include <vagra/page.h>

namespace vagra
{

void Page::Init(const std::vector<unsigned int>& cont_ids, const unsigned int arg_page, const unsigned int arg_amount)
{
	page = arg_page;
	amount = arg_amount;

	if(page == 0)
		page = 1;

	if(amount == 0)
	{
		Nexus& nx = Nexus::getInstance();
		amount = nx.getConfig("page_size", nx.getPageSize());
		if(amount == 0)
			amount = 10;
	}
		
	unsigned int last_cont = amount * page;
	unsigned int first_cont = last_cont - amount;

	last = cont_ids.size() / amount;
	if(cont_ids.size() % amount)
		last++;

	if(last_cont > cont_ids.size())
		last_cont = cont_ids.size();
	if(last_cont > first_cont)
	{
		cont.resize(last_cont - first_cont);
		std::copy(cont_ids.begin()+first_cont, cont_ids.begin()+last_cont, cont.begin());
	}
}

Page::Page(const std::vector<unsigned int>& cont_ids, const unsigned int arg_page, const unsigned int arg_amount)
{
	Init(cont_ids, arg_page, arg_amount);
}

int Page::getCurrent() const
{
	return page;
}

int Page::getNext() const
{
	if(page < last)
		return page + 1;
	return page;
}

int Page::getPrevious() const
{
	if(page > 1)
		return page - 1;
	return page;
}

int Page::getFirst() const
{
	return 1;
}

int Page::getLast() const
{
	return last;
}

int Page::getAmount() const
{
	return last;
}

Page::size_type Page::size() const
{
	return cont.size();
}

Page::const_iterator Page::begin() const
{
	return cont.begin();
}

Page::const_iterator Page::end() const
{
	return cont.end();
}

} //namespace vagra
