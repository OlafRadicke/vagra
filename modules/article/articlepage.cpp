#include <vagra/nexus.h>
#include <vagra/article/articlecache.h>
#include <vagra/article/articlepage.h>

namespace vagra
{

ArtPage::ArtPage (const std::vector<unsigned int>& cont_ids, unsigned int arg_page, unsigned int arg_amount)
{
	if(arg_amount == 0)
	{
		Nexus& nx = Nexus::getInstance();
		arg_amount = nx.getConfig("article_page_size", nx.getPageSize());
	}
	Init(cont_ids, arg_page, arg_amount);
}

ArtPage::ArtPage(unsigned int arg_page, unsigned int arg_amount)
{
	ArticleCache& art_cache = ArticleCache::getInstance();
	if(arg_amount == 0)
	{
		Nexus& nx = Nexus::getInstance();
		arg_amount = nx.getConfig("article_page_size", nx.getPageSize());
	}
	Init(art_cache.getInv(), arg_page, arg_amount);
}

} //namespace vagra
