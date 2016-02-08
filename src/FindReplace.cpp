#include "precompiled.h"
#include "FindReplace.h"

#include "globals.h"

struct FindReplaceImpl
{
	enum { MaxTermLength = 256 };

	TCHAR		searchTerm[MaxTermLength];
	TCHAR		replaceTerm[MaxTermLength];
	HWND		hwnd;							// find/replace dialog window
	HWND		hwndEdit;						// edit window currently associated
	FINDREPLACE	fr;								// struct for windows Find/ReplaceText APIs
	FINDTEXTEX	ft;								// 
};


FindReplace* FindReplace::Instance()
{
	static FindReplace instance;

	return &instance;
}


FindReplace::FindReplace() : m_impl(new FindReplaceImpl)
{
	Initialize();
}


FindReplace::~FindReplace()
{
	delete m_impl;
}


void FindReplace::Initialize()
{
	*m_impl->searchTerm = 0;
	*m_impl->replaceTerm = 0;
	m_impl->ft.lpstrText = m_impl->searchTerm;
	m_impl->ft.chrgText.cpMin = 0;
	m_impl->ft.chrgText.cpMax = 0;

//	memset(&m_impl->fr, 0, sizeof(m_impl->fr));
	m_impl->fr.Flags = FR_DOWN;
	m_impl->fr.lStructSize = sizeof(m_impl->fr);
	m_impl->fr.hInstance = GetModuleHandle(0);
	m_impl->fr.lpstrFindWhat = m_impl->searchTerm;
	m_impl->fr.wFindWhatLen = FindReplaceImpl::MaxTermLength;
	m_impl->fr.lpstrReplaceWith = m_impl->replaceTerm;
	m_impl->fr.wReplaceWithLen = FindReplaceImpl::MaxTermLength;
}


void FindReplace::InitializeFindReplace(int start, int end)
{
	if(m_impl->hwnd)
		DestroyWindow(m_impl->hwnd);

	m_impl->ft.chrgText.cpMin = m_impl->ft.chrgText.cpMax = 0;
	m_impl->ft.chrg.cpMin = start;
	m_impl->ft.chrg.cpMax = end;
	m_impl->fr.Flags &= ~FR_DIALOGTERM;
}


HWND FindReplace::GetHwnd() const
{
	return m_impl->hwnd;
}


void FindReplace::SetOwnerWindow(HWND hwndOwner, HWND hwndEdit)
{
	m_impl->fr.hwndOwner = hwndOwner;
	m_impl->hwndEdit = hwndEdit;
}


bool FindReplace::ProcessMessage(unsigned msg)
{
	if( msg == GetFindReplaceMsg() )
	{
		if(m_impl->fr.Flags & FR_DIALOGTERM)
		{
			m_impl->hwnd = 0;
			return false;
		}

		return true;
	} else
		return false;
}


bool FindReplace::MessageLoopHook(MSG &msg)
{
	if(m_impl->hwnd)
		return (0 != ::IsDialogMessage(m_impl->hwnd, &msg) );
	else
		return false;
}


void FindReplace::StartFind(int start, int end)
{
	InitializeFindReplace(start, end);
	m_impl->hwnd = ::FindText(&m_impl->fr);
}


void FindReplace::StartReplace(int start, int end)
{
	InitializeFindReplace(start, end);
	m_impl->hwnd = ::ReplaceText(&m_impl->fr);
}


int FindReplace::GetMatchStart() const
{
	return m_impl->ft.chrgText.cpMin;
}


int FindReplace::GetMatchEnd() const
{
	return m_impl->ft.chrgText.cpMax;
}


int FindReplace::NextFind()
{
	const unsigned findFlags = m_impl->fr.Flags & (FR_DOWN | FR_MATCHCASE | FR_WHOLEWORD);

	int result = static_cast<int>(
		SendMessage(m_impl->hwndEdit, EM_FINDTEXTEX, findFlags, reinterpret_cast<LPARAM>( &m_impl->ft )) );

	const CHARRANGE &match = m_impl->ft.chrgText;
	m_impl->ft.chrg.cpMin = (match.cpMax == -1) ? 0 : match.cpMax;

	return result;
}


int FindReplace::NextReplace()
{
	return -1;
}


const TCHAR* FindReplace::GetSearchTerm() const
{
	return m_impl->searchTerm;
}


const TCHAR* FindReplace::GetReplaceTerm() const
{
	return m_impl->replaceTerm;
}



unsigned FindReplace::GetFindReplaceMsg()
{
	return g_findReplaceMsg;
}
