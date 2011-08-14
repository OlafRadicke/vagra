#ifndef VARGA_CONFIG_H
#define VARGA_CONFIG_H

#include <string>

#include <vagra/nexus.h>

namespace vagra
{

template <typename T>
	T getConfig(const std::string& confstr, const T& defval)
{
	Nexus& nx = Nexus::getInstance();
	return nx.getConfig(confstr, defval);
}

std::string getConfig(const std::string&);

} //namespace vagra

#endif // VARGA_CONFIG_H
