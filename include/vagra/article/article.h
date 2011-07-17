#ifndef VARGA_ARTICLE_H
#define VARGA_ARTICLE_H

#include <string>
#include <vector>

#include <vagra/types.h>
#include <vagra/baseobject.h>

namespace vagra
{

class Article: public BaseObject
{
	std::string url;
	std::vector<std::string> tags;
	std::vector<unsigned int> comment_ids;

    public:
	Article() {}
	explicit Article(const std::string&);
	explicit Article(const unsigned int);
	Article* operator->() { return this; }
	void clear();

	const std::string& getUrl() const;
	const std::vector<std::string>& getTags() const;
	std::string getTagString() const;
	std::vector<unsigned int> getCommentIds() const;
	const szt_vecint getCommentAmount() const;

	void setTags(const std::string&);
	void dbInsert();
	void dbUpdate();
};

unsigned int getArticleIdByTitle (const std::string&);
unsigned int getArticleIdByTitle (const std::string&, dbconn&);

} //namespace vagra

#endif // VAGRA_ARTICLE_H
