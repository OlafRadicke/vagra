#ifndef VARGA_COMMENTPAGE_H
#define VARGA_COMMENTPAGE_H

#include <vagra/page.h>
#include <vagra/comment/cachedcomment.h>

namespace vagra
{

class CommentPage: public Page<CachedComment>
{
    public:
	CommentPage(const std::vector<unsigned int>&, unsigned int, unsigned int = 0);
};

} //namespace vagra

#endif // VARGA_COMMENTPAGE_H
