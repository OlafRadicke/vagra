#include <tntdb/connect.h>
#include <cxxtools/membar.h>
#include <cxxtools/mutex.h>

#include <vagra/nexus.h>

namespace vagra
{

//begin Nexus

Nexus::Nexus()
{
	std::string vagraConf;
	const char* vagraConfEnv = ::getenv("VAGRA_CONF");

	if(vagraConfEnv)
		vagraConf = vagraConfEnv;
	else
		vagraConf = "vagra.conf";

	config.load(vagraConf.c_str());

	db_conn_s = getConfig("db_conn_s");
	db_connpool_size = getConfig("db_connpool_size", 32);
	cache_size = getConfig("cache_size", 100);
	page_size = getConfig("page_size", 10);

	tntdb::setMaxPoolSize(db_connpool_size);
}

Nexus& Nexus::getInstance()
{
	static Nexus* volatile instance = NULL;
	static cxxtools::Mutex mutex;

	cxxtools::membar_read();
	if(!instance)
	{
		cxxtools::MutexLock lock(mutex);
		cxxtools::membar_read();
		if(!instance)
		{
			Nexus* _tmp = new Nexus();
			cxxtools::membar_write();;
			instance = _tmp;
		}
	}
	return *instance;
}

std::string Nexus::getConfig(const std::string& confstr)
{
	std::string defval;
	return getConfig(confstr, defval);
}

unsigned int Nexus::getCacheSize() const
{
	return cache_size;
}

unsigned int Nexus::getPageSize() const
{
	return page_size;
}

dbconn Nexus::dbConnection() const
{
	return tntdb::connectCached(db_conn_s);
}

//end Nexus

} //namespace vagra
