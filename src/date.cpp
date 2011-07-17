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
