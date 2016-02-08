#pragma once
#ifndef idC7D2CEC3_29ED_4991_93A6EAB8A3DEDDDB
#define idC7D2CEC3_29ED_4991_93A6EAB8A3DEDDDB

struct FindReplaceImpl;


class FindReplace
{
public:
	static		FindReplace* Instance();
	~FindReplace();

	HWND		GetHwnd() const;
	void		SetOwnerWindow(HWND hwndOwner, HWND hwndEdit);
	bool		ProcessMessage(unsigned msg);
	bool		MessageLoopHook(MSG &msg);

	void		StartFind(int start, int end);
	void		StartReplace(int start, int end);

	int			NextFind();
	int			NextReplace();

	int			GetMatchStart() const;
	int			GetMatchEnd() const;


	const TCHAR* GetSearchTerm() const;
	const TCHAR* GetReplaceTerm() const;

	static unsigned GetFindReplaceMsg();


private:
	void	Initialize();
	void	InitializeFindReplace(int start, int end);

	FindReplaceImpl	*m_impl;

	// disallow construction, copy construction, assignment
	FindReplace();
	FindReplace(const FindReplace&);
	FindReplace& operator= (const FindReplace&);
};



#endif // header
