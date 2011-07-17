#ifndef VARGA_ARTICLEPAGE_H
#define VARGA_ARTICLEPAGE_H

#include <vagra/page.h>
#include <vagra/article/cachedarticle.h>

namespace vagra
{

class ArtPage: public Page<CachedArticle>
{
    public:
	ArtPage(const std::vector<unsigned int>&, unsigned int, unsigned int = 0);
	ArtPage(unsigned int, unsigned int = 0);
};

} //namespace vagra

#endif // VARGA_ARTICLEPAGE_H
