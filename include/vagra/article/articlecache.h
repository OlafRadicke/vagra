#ifndef VARGA_ARTICLECACHE_H
#define VARGA_ARTICLECACHE_H

#include <map>
#include <vector>

#include <cxxtools/mutex.h>

#include <vagra/types.h>
#include <vagra/basecache.h>
#include <vagra/date.h>

#include <vagra/article/article.h>

namespace vagra
{

typedef std::vector<std::pair<std::string, unsigned int> > tsum;

class ArticleCache: public BaseCache<Article>
{
	ArticleCache();

        unsigned int getCacheSize()
        {
                Nexus& nx = Nexus::getInstance();
                return nx.getConfig("art_cache_size", nx.getCacheSize());
        }

        std::map<std::string, unsigned int> id_map;
        std::vector<unsigned int> inventory;
	tsum tagsum;
	vdate mtime;

        cxxtools::ReadWriteMutex id_map_mutex;
	cxxtools::ReadWriteMutex tsum_mutex;
        cxxtools::ReadWriteMutex id_inv_mutex;
	cxxtools::ReadWriteMutex mtime_mutex;

	void initInv();

    public:
	typedef std::vector<unsigned int>::size_type size_type;

        static ArticleCache& getInstance();
        unsigned int getIdByTitle(const std::string&);

	std::vector<unsigned int> getInv();
	std::vector<unsigned int> getInv(unsigned int, unsigned int);
	size_type invSize() const;
	tsum getTagsum();
	vdate getMTime();

	void invAdd(unsigned int);
	void invRemove(unsigned int);
	void updateTagsum();
	void updateMTime();
};

typedef ArticleCache::SharedObject SharedArticle;

unsigned int cachedGetArticleIdByTitle(const std::string&);
tsum getTagsum();
ArticleCache::size_type getArticleAmount();

std::vector<unsigned int> searchArtByTag(const std::string&);

vdate updated();

} //namespace vagra

#endif // VARGA_ARTICLECACHE_H
