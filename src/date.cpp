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
#include <sstream>
#include <iomanip>
#include <string>
#include <stdexcept>

#include <vagra/date.h>

namespace vagra
{

const std::string date2dstr(const vdate& d)
{
	std::ostringstream s;
	s << d.getDay();
	switch(d.getMonth())
	{
		case 1:
			s << gettext(" Jan ");
			break;
		case 2:
			s << gettext(" Feb ");
			break;
		case 3:
			s << gettext(" Mar ");
			break;
		case 4:
			s << gettext(" Apr ");
			break;
		case 5:
			s << gettext(" May ");
			break;
		case 6:
			s << gettext(" Jun ");
			break;
		case 7:
			s << gettext(" Jul ");
			break;
		case 8:
			s << gettext(" Aug ");
			break;
		case 9:
			s << gettext(" Sep ");
			break;
		case 10:
			s << gettext(" Oct ");
			break;
		case 11:
			s << gettext(" Nov ");
			break;
		case 12:
			s << gettext(" Dec ");
			break;
		default:
			throw std::domain_error(gettext("invalid date"));
			break;
	}
	s << d.getYear();
	return s.str();
}

const std::string date2cdstr(const vdate& d)
{
	// YYYY-MM-DDTHH:MM
	std::ostringstream s;
	s << std::setw(4) << std::setfill('0') << d.getYear();
        s << "-" << std::setw(2) << std::setfill('0') << d.getMonth();
	s << "-" << std::setw(2) << std::setfill('0') << d.getDay();
        s << "T" << std::setw(2) << std::setfill('0') << d.getHour();
        s << ":" << std::setw(2) << std::setfill('0') << d.getMinute();
	s << ":" << std::setw(2) << std::setfill('0') << d.getSecond();
	s << "Z";
	return s.str();
}

const std::string date2str(const vdate& d)
{
	if(d.isNull())
		return "";
	std::ostringstream s;
	s << date2dstr(d);
	s << ", " << std::setw(2) << d.getHour() << ":" 
		<< std::setw(2) << std::setfill('0') << d.getMinute();
	return s.str();
}

} //namespace vagra
