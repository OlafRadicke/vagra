#ifndef VARGA_CACHEDARTICLE_H
#define VARGA_CACHEDARTICLE_H

#include <vagra/types.h>

#include <vagra/article/articlecache.h>

namespace vagra
{

class CachedArticle
{
	CachedArticle() {}
	SharedArticle art;

    public:
	explicit CachedArticle(const unsigned int);
	explicit CachedArticle(const std::string&);
	operator bool() const;

	const SharedArticle& operator->() const { return art; }
	Article operator*() const { return *art; }
};

} //namespace vagra

#endif // VARGA_CACHEDARTICLE_H
