#include <vagra/baseobject.h>

namespace vagra
{

//begin BaseObject

BaseObject::operator bool() const
{
	return(!title.empty() && !head.empty() && (!text.empty() || !abstract.empty()));
}

void BaseObject::clearBase()
{
	id = 0;
	title.clear();
	head.clear();
	abstract.clear();
	text.clear();
	author.clear();
	ctime = vdate();
	mtime = vdate();
}

const unsigned int BaseObject::getId() const
{
	return id;
}

const std::string& BaseObject::getTitle() const
{
	return title;
}

const std::string& BaseObject::getHead() const
{
	return head;
}

const std::string& BaseObject::getAbstract() const
{
	return abstract;
}

const std::string& BaseObject::getText() const
{
	return text;
}

const std::string& BaseObject::getAuthor() const
{
	return author;
}

const vdate& BaseObject::getCTime() const
{
	return ctime;
}

const vdate& BaseObject::getMTime() const
{
	return mtime;
}

void BaseObject::setTitle(const std::string& s)
{
	title = s;
}

void BaseObject::setHead(const std::string& s)
{
	head = s;
}

void BaseObject::setAbstract(const std::string& s)
{
	abstract = s;
}

void BaseObject::setText(const std::string& s)
{
	text = s;
}

void BaseObject::setAuthor(const std::string& s)
{
	author = s;
}

//end BaseObject

} //namespace vagra
