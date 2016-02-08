#pragma once
#ifndef id9D7078D2_A35E_4FB3_98AB3983D0C191E8
#define id9D7078D2_A35E_4FB3_98AB3983D0C191E8

#include <tchar.h>
#include <iterator>
#include <vector>

/************************************************************************/
/* IconItem: a single icon entry from a .ico file or PE resource.       */
/************************************************************************/
class IconItemImpl;
class IconItem
{
public:
	IconItem();
	~IconItem();

	IconItem(const IconItem& rhs);
	IconItem& operator= (const IconItem& rhs);

	void	copyFrom(const IconItem& rhs);

	int		get_cx();
	int		get_cy();
	int		get_bpp();
	void	get_rawdata(std::vector<unsigned char>& data);

	void	set_cx(int cx);
	void	set_cy(int cy);
	void	set_bpp(int bpp);
	void	set_rawdata(std::vector<unsigned char>& data);

private:
	IconItemImpl	*m_imp;
};



/************************************************************************/
/* IconGroup: a collection of IconItems - represents a full .ico file,  */
/* or an icon group from a PE resource									*/
/************************************************************************/
class IconGroupImpl;
class IconGroup
{
public:
	class IconIterator : public std::iterator<std::bidirectional_iterator_tag, IconItem>
	{
	public:
		inline IconIterator();
		inline IconIterator(IconGroup *container, int index);
		inline IconIterator(const IconIterator& rhs);

		inline IconIterator& operator= (const IconIterator& rhs);

		inline IconIterator& operator++ ();		// prefix
		inline const IconIterator operator++(int);	// postfix
		inline IconIterator& operator-- ();		// prefix
		inline const IconIterator operator--(int);	// postfix

		inline IconItem& operator* ();
		inline IconItem* operator->();

		inline bool operator== (const IconIterator& rhs);
		inline bool operator!= (const IconIterator& rhs);

	private:
		IconGroup	*m_container;
		size_t		m_index;
	};
	typedef	IconIterator iterator;

	IconGroup();
	~IconGroup();

	iterator	begin();
	iterator	end();
	size_t		size();

	//	iterator find_match(int cx, int cy, int bpp);
	bool		load_from_icofile(TCHAR *filename);


private:
	IconGroupImpl	*m_impl;

	IconItem&	getItem(int index);

	// disallow copy construction and assignment
	IconGroup(const IconGroup&);
	IconGroup& operator= (const IconGroup&);
};


/************************************************************************/
/* Inline definition of the IconIterator class                          */
/************************************************************************/
IconGroup::IconIterator::IconIterator() :
	m_container(0), m_index(0)
{
}


IconGroup::IconIterator::IconIterator(IconGroup *container, int index) :
	m_container(container), m_index(index)
{
}


IconGroup::IconIterator::IconIterator(const IconIterator& rhs) :
	m_container(rhs.m_container), m_index(rhs.m_index)
{
}


IconGroup::IconIterator& IconGroup::IconIterator::operator= (const IconIterator& rhs)
{
	m_container = rhs.m_container;
	m_index = rhs.m_index;
	return *this;
}


IconGroup::IconIterator& IconGroup::IconIterator::operator++ ()			// prefix
{
	if(m_index < m_container->size())
		++m_index;
	return *this;
}
const IconGroup::IconIterator IconGroup::IconIterator::operator++(int)	// postfix
{
	const IconIterator old(*this);
	++(*this);
	return old;
}


IconGroup::IconIterator& IconGroup::IconIterator::operator-- ()			// prefix
{
	if(m_index > 0)
		--m_index;
	return *this;
}
const IconGroup::IconIterator IconGroup::IconIterator::operator--(int)	// postfix
{
	const IconIterator old(*this);
	--(*this);
	return old;
}


IconItem& IconGroup::IconIterator::operator* ()
{
	return m_container->getItem(m_index);
}
IconItem* IconGroup::IconIterator::operator->()
{
	return &**this;
}


bool IconGroup::IconIterator::operator== (const IconIterator& rhs)
{
	return (m_container == rhs.m_container) && (m_index == rhs.m_index);
}
bool IconGroup::IconIterator::operator!= (const IconIterator& rhs)
{
	return !(*this == rhs);
}

#endif // header
