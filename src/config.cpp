#include <vagra/config.h>

namespace vagra
{

std::string getConfig(const std::string& confstr)
{
	Nexus& nx = Nexus::getInstance();
	return  nx.getConfig(confstr);
}

} //namespace vagra
