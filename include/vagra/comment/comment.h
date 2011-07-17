#ifndef VARGA_COMMENT_H
#define VARGA_COMMENT_H

#include <string>

#include <vagra/types.h>
#include <vagra/baseobject.h>

namespace vagra
{

class Comment: public BaseObject
{
	unsigned int ref_id;
	unsigned int art_id;
	std::string homepage;
	std::string mail;

    public:
	Comment() {}
	explicit Comment(const unsigned int);
	operator bool() const;
	Comment* operator->() { return this; }
	void clear();

	const unsigned int getRef() const;
	const unsigned int getArticle() const;
	const std::string& getHomepage() const;
	const std::string& getMail() const;

	void setRef(const unsigned int);
	void setArticle(const unsigned int);
	void setHomepage(const std::string&);
	void setMail(const std::string&);
	void dbInsert();
	void dbUpdate();
};

} //namespace vagra

#endif // VAGRA_COMMENT_H
