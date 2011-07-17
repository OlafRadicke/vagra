#ifndef VARGA_NEXUS_H
#define VARGA_NEXUS_H

#include <string>

#include <cxxtools/noncopyable.h>
#include <tnt/tntconfig.h>

#include <vagra/types.h>

namespace vagra
{

class Nexus: private cxxtools::NonCopyable
{
	Nexus();

	tnt::Tntconfig config;

	std::string db_conn_s;
	unsigned int db_connpool_size;
	unsigned int cache_size;
	unsigned int page_size;

    public:
	static Nexus& getInstance();

	std::string getConfig(const std::string& confstr);

	template <typename T>
		T getConfig(const std::string& confstr, const T& defval)
	{
		return config.getValue(confstr, defval);
	}

	dbconn dbConnection() const;
	unsigned int getPageSize() const;
	unsigned int getCacheSize() const;
};

} //namespace vagra

#endif // VARGA_NEXUS_H
