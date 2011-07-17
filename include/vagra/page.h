#ifndef VARGA_PAGE_H
#define VARGA_PAGE_H

#include <vector>

namespace vagra
{

template <typename Content>
class Page
{
    protected:
	Page() {}
	std::vector<Content> cont;
        
        unsigned int page;
        unsigned int amount;
        unsigned int last;

	void Init(const std::vector<unsigned int>& cont_ids, unsigned int arg_page, unsigned int arg_amount)
	{
		page = arg_page;
		amount = arg_amount;

		if(page == 0)
			page = 1;

		unsigned int last_cont = amount * page;
		unsigned int first_cont = last_cont - amount;

		last = cont_ids.size() / amount;
		if(cont_ids.size() % amount)
			last++;

		if(last_cont > cont_ids.size())
			last_cont = cont_ids.size();
		if(last_cont > first_cont)
		{
			std::vector<unsigned int> inv;
			inv.resize(last_cont - first_cont);
			std::copy(cont_ids.begin()+first_cont, cont_ids.begin()+last_cont, inv.begin());

			for (std::vector<unsigned int>::const_iterator it = inv.begin();
				it != inv.end(); it++)
			{
				cont.push_back(Content(*it));
	                }
       		}
	}

    public:
        typedef typename std::vector<Content>::const_iterator const_iterator;
        typedef typename std::vector<Content>::size_type size_type;
	
	Page (const std::vector<unsigned int>& cont_ids, unsigned int arg_page, unsigned int arg_amount)
	{
		Init(cont_ids, arg_page, arg_amount);
	}

	virtual ~Page() {}
	
	int getCurrent() const
	{
		return page;
	}
	
	int getNext() const
	{
		if(page < last)
			return page + 1;
		return page;
	}
	
	int getPrevious() const
	{
		if(page > 1)
			return page - 1;
		return page;
	}
	
	int getFirst() const
	{
		return 1;
	}
	
	int getLast() const
	{
		return last;
	}
	
	int getAmount() const
	{
		return last;
	}
	
	size_type size() const
	{
		return cont.size();
	}
	
	typename std::vector<Content>::const_iterator begin() const
	{
		return cont.begin();
	}
	
	typename std::vector<Content>::const_iterator end() const
	{
		return cont.end();
	}
};

} //namespace vagra

#endif // VARGA_PAGE_H
