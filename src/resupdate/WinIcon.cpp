#include <algorithm>

#include "WinIcon.h"
#include "WinIconInternal.h"
/************************************************************************/
/* Implementation of IconItem                                           */
/************************************************************************/
class IconItemImpl
{
public:
	GRPICONDIRENTRY				iconInfo;
	std::vector<unsigned char>	rawData;
};


IconItem::IconItem() :
	m_imp(new IconItemImpl)
{
}


IconItem::~IconItem()
{
	delete m_imp;
}


IconItem::IconItem(const IconItem& rhs) :
	m_imp(new IconItemImpl)
{
	copyFrom(rhs);
}


IconItem& IconItem::operator= (const IconItem& rhs)
{
	copyFrom(rhs);
	return *this;
}


void IconItem::copyFrom(const IconItem& rhs)
{
	m_imp->iconInfo = rhs.m_imp->iconInfo;
	m_imp->rawData = rhs.m_imp->rawData;
}


int	IconItem::get_cx()
{
	return m_imp->iconInfo.bWidth;
}


int	IconItem::get_cy()
{
	return m_imp->iconInfo.bHeight;
}


int	IconItem::get_bpp()
{
	return m_imp->iconInfo.wBitCount;
}


void IconItem::get_rawdata(std::vector<unsigned char>& data)
{
	data = m_imp->rawData;
	// data.assign(m_imp->rawData.begin(), m_imp->rawData.end());
}


void IconItem::set_cx(int cx)
{
	m_imp->iconInfo.bWidth = cx;
}


void IconItem::set_cy(int cy)
{
	m_imp->iconInfo.bHeight = cy;
}


void IconItem::set_bpp(int bpp)
{
	m_imp->iconInfo.wBitCount = bpp;
}


void IconItem::set_rawdata(std::vector<unsigned char>& data)
{
	m_imp->rawData = data;
}


/************************************************************************/
/* Implementation of IconGroup                                          */
/************************************************************************/
class IconGroupImpl
{
public:
	std::vector<IconItem>	itemCont;
};


IconGroup::IconGroup() :
	m_impl(new IconGroupImpl)
{
}


IconGroup::~IconGroup()
{
	delete m_impl;
}


IconGroup::iterator IconGroup::begin()
{
	return iterator(this, 0);
}


IconGroup::iterator IconGroup::end()
{
	return iterator(this, size());
}


size_t IconGroup::size()
{
	return m_impl->itemCont.size();
}



IconItem& IconGroup::getItem(int index)
{
	return m_impl->itemCont[index];
}


bool	IconGroup::load_from_icofile(TCHAR *filename)
{
	ICONDIR			idir;

	FILE	*f = _tfopen(filename, _T("rb"));
	if(!f)
		return false;

	fread(&idir, 1, sizeof(idir), f);
	if ((idir.idReserved != 0) || (idir.idType != 1))
		return false;

	ICONDIRENTRY	ient;
	IconItem		iitem;
	for(size_t idx=0; idx<idir.idCount; idx++)
	{
		fread(&ient, 1, sizeof(ient), f);
		iitem.set_cx(ient.bWidth);
		iitem.set_cy(ient.bHeight);
		iitem.set_bpp(ient.bColorCount);

		m_impl->itemCont.push_back(iitem);
	}

	fclose(f);
	return true;
}
