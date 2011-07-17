#ifndef VARGA_TYPES_H
#define VARGA_TYPES_H

#include <vector>
#include <tntdb/connection.h>

namespace vagra
{

typedef std::vector<unsigned int>::size_type szt_vecint;
typedef tntdb::Connection dbconn;

} // namespace vagra

#endif // VAGRA_TYPES_H
