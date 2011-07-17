#ifndef VARGA_CACHEDCOMMENT_H
#define VARGA_CACHEDCOMMENT_H

#include <vagra/cache.h>
#include <vagra/comment/comment.h>

namespace vagra
{

class CachedComment
{
	CachedComment() {}
	Cache<Comment>::SharedObject comment;

    public:
	explicit CachedComment(const unsigned int);
	operator bool() const;

	const Cache<Comment>::SharedObject& operator->() const { return comment; }
	Comment operator*() const { return *comment; }
};

} //namespace vagra

#endif // VARGA_CACHEDCOMMENT_H
