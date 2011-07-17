#ifndef VARGA_BAASEOBJECT_H
#define VARGA_BAASEOBJECT_H

#include <string>
#include <vector>

#include <vagra/types.h>
#include <vagra/date.h>

namespace vagra
{

class BaseObject
{
	virtual void dbInsert() = NULL;
	virtual void dbUpdate() = NULL;
	virtual void clear() = NULL;

    protected:
	BaseObject() {}
	unsigned int id;
	std::string title;
	std::string head;
	std::string abstract;
	std::string text;
	std::string author;
	vdate ctime;
	vdate mtime;
	void clearBase();

    public:
	virtual operator bool() const;
	virtual BaseObject* operator->() = NULL;
	virtual ~BaseObject() {}

	const unsigned int getId() const;
	const std::string& getTitle() const;
	const std::string& getHead() const;
	const std::string& getAbstract() const;
	const std::string& getText() const;
	const std::string& getAuthor() const;
	const vdate& getCTime() const;
	const vdate& getMTime() const;

	void setTitle(const std::string&);
	void setHead(const std::string&);
	void setAbstract(const std::string&);
	void setText(const std::string&);
	void setAuthor(const std::string&);
};

} //namespace vagra

#endif // VAGRA_VARGA_BAASEOBJECT_H
