#include <stdexcept>
#include <libintl.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>

#include <vagra/article/cachedarticle.h>

namespace vagra
{

CachedArticle::CachedArticle(const unsigned int art_id)
{
	ArticleCache& art_cache = ArticleCache::getInstance();
	art = art_cache.get(art_id);
}

CachedArticle::CachedArticle(const std::string& art_title)
{
	ArticleCache& art_cache = ArticleCache::getInstance();
	art = art_cache.get(cachedGetArticleIdByTitle(art_title));
}

CachedArticle::operator bool() const
{
	return *art;
}

} //namespace vagra
