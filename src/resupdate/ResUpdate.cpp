#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "WinIcon.h"





/*
class ResourceUpdater
{
public:
	ResourceUpdater(const TCHAR *exepath);
	~ResourceUpdater();

	bool	update(LPCTSTR type, LPCTSTR id, WORD lang, std::vector<BYTE> data);

private:
	// disallow copy construction and assignment
	ResourceUpdater(const ResourceUpdater&);
	ResourceUpdater& operator= (const ResourceUpdater&);
};


class ResourceIconUpdater
{
public:
	ResourceIconUpdater(const TCHAR *exepath);
	~ResourceIconUpdater();

	bool	update(LPCTSTR id, WORD lang, std::vector<BYTE> data);

private:
	// disallow copy construction and assignment
	ResourceIconUpdater(const ResourceIconUpdater&);
	ResourceIconUpdater& operator= (const ResourceIconUpdater&);
};
*/


#include <stdio.h>

int main()
{
	IconGroup	ig;
	IconGroup::iterator it;

	ig.load_from_icofile(_T("c:\\test.ico"));

	printf("IconGroup has %d elements\n", ig.size());
	for(it = ig.begin(); it != ig.end(); ++it)
	{
		printf("Element is %d*%d*%d\n", it->get_cx(), it->get_cy(), it->get_bpp());
	}
}
