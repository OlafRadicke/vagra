#include <vagra/comment/cachedcomment.h>

namespace vagra
{

CachedComment::CachedComment(const unsigned int comm_id)
{
	Cache<Comment>& comm_cache = Cache<Comment>::getInstance();
	comment = comm_cache.get(comm_id);
}

CachedComment::operator bool() const
{
	return *comment;
}

} //namespace vagra
