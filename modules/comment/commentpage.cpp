#include <vagra/nexus.h>
#include <vagra/comment/commentpage.h>

namespace vagra
{

CommentPage::CommentPage (const std::vector<unsigned int>& cont_ids, unsigned int arg_page, unsigned int arg_amount)
{
	if(arg_amount == 0)
	{
		Nexus& nx = Nexus::getInstance();
		arg_amount = nx.getConfig("comment_page_size", nx.getPageSize());
	}
	Init(cont_ids, arg_page, arg_amount);
}

} //namespace vagra
