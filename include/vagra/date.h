#ifndef VARGA_DATE_H
#define VARGA_DATE_H

#include <string>
#include <tntdb/datetime.h>

namespace vagra
{

typedef tntdb::Datetime vdate;
const std::string date2dstr(const vdate& d);
const std::string date2cdstr(const vdate& d);
const std::string date2str(const vdate& d);

} //namespace vagra

#endif // VARGA_DATE_H
