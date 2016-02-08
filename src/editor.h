#pragma once
#ifndef idB8EDEABA_421B_41A8_B331B0DC5CC60AE6
#define idB8EDEABA_421B_41A8_B331B0DC5CC60AE6

#include "Document.h"

class Plaintext;

namespace editor
{

struct EditorImpl;

class Editor
{
public:
	Editor(std::auto_ptr<document::Document> &doc);
	~Editor();

	bool	IsModified() const;
	HWND	GetHWND() const;

	void	SetPosition(unsigned x, unsigned y);
	void	SetSize(unsigned w, unsigned h);

	size_t	GetTextLength();
	void	GetText(Plaintext& text);
	void	SetText(const Plaintext& text);

	void	UpdateTitle(const TCHAR* title=0);

	LRESULT	Dispatch(HWND hwnd, unsigned msg, WPARAM wParam, LPARAM lParam);

private:
	// disallow copy construction and assignment
	Editor(const Editor&);
	Editor operator= (const Editor&);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// private member functions
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void setFont(HFONT font);
	void loadFontSettings();

	/**
	* Enables/disables file menu items based on read-only state etc
	*/
	void	PrepareFileMenu(HMENU menu);

	/**
	* Enables/disables edit menu items based on RichEdit state
	*/
	void	PrepareEditMenu(HMENU menu);

	bool	DoImport(const xstring& filename);
	bool	DoExport(const xstring& filename);

	// message handlers
	void OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized);
	void OnClose(HWND hwnd);
	void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
	void OnDestroy(HWND hwnd);
	void OnDropFiles(HWND hwnd, HDROP hDropInfo);
	void OnSize(HWND hwnd, UINT state, int cx, int cy);
	void OnFindReplace(HWND hwnd, const FINDREPLACE &fr);
	void OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
	LRESULT OnNotify(HWND hwnd, int ctrl, LPNMHDR nmh);


	/**
	* If document is modified, ask user to save/discard/cancel, and act accordingly.
    * @return true: user chose save or discard. false: user chose cancel or save failed.
	*/
	bool	CheckAndHandleModified();

	void	handle_new();
	bool	handle_save();
	bool	handle_saveas(bool readonly);
	void	handle_import();
	void	handle_export();
	void	handle_setkey();
	void	handle_readonly();
	void	handle_exit();
	void	handle_about();
	void	handle_font();
	void	handle_find();
	void	handle_findnext();
	void	handle_replace();
	void	handle_richpopup(int x, int y);
	void	handle_richlink(const ENLINK& eli);

	// data members
	EditorImpl *m_impl;
};



} // editor namespace

#endif // header
