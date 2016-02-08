#include "precompiled.h"
#include "editor.h"

#include "globals.h"
#include "resource.h"
#include "DlgAbout.h"
#include "DlgPass2.h"
#include "FindReplace.h"
#include "FileEndData.h"
#include "sekrit.h"
#include "font.h"
#include "util.h"
#include "Plaintext.h"

namespace editor
{

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

const unsigned WIN_WIDTH = 480;						// default window width
const unsigned WIN_HEIGHT = 320;					// default window height

const unsigned MAX_EDIT_SIZE = 1024*1024*4;			// maximum size of text supported

struct EditorImpl
{
	EditorImpl() : hwnd(0), hwndEdit(0), font(0), doc(0)
	{
	}

	~EditorImpl()
	{
		if(font)
		{
			// set system default font so our font object is no longer in use,
			// then dispose of the font.
			SendMessage(hwndEdit, WM_SETFONT, 0, FALSE);
			DeleteObject(font);
		}
	}

	HWND		hwnd;			// main window handle
	HWND		hwndEdit;		// edit window handle
	HFONT		font;			// font handle for edit window

	std::auto_ptr<document::Document>	doc;
};


Editor::Editor(std::auto_ptr<document::Document> &doc) : m_impl(new EditorImpl)
{
	m_impl->doc = doc;

	static TCHAR const* className = _T("fSekrit::main");
	static WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, WindowProc, 0, 0,
			::GetModuleHandle(0), 0, ::LoadCursor(0, IDC_ARROW), 0,
			MAKEINTRESOURCE(IDR_MENU1), className, 0
	};

	if(! ::RegisterClassEx(&wc))
		throw;

	unsigned w = m_impl->doc->GetWidth();
	unsigned h = m_impl->doc->GetHeight();

	// m_impl->hwnd will be set in WM_CREATE handler, so don't set it here. This
	// is a necessary evil, since the WM_CREATE handler might need access to the
	// hwnd - and m_hwnd will of course not be set until WM_CREATE is done.
	::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, className, 0, WS_OVERLAPPEDWINDOW,
		0, 0, w ? w : WIN_WIDTH, h ? h : WIN_HEIGHT,
		0, 0, wc.hInstance, this);

	if(!m_impl->hwnd)
		throw;

	DragAcceptFiles(m_impl->hwnd, TRUE);

	// get plaintext, set plaintext in editor window
	Plaintext temp;
	m_impl->doc->GetText(temp);
	SetText(temp);

	::SendMessage(m_impl->hwndEdit, EM_SETMODIFY, 0, 0);

	UpdateTitle(strNewDocument);

	if(m_impl->doc->GetMaximized() )
	{
		::ShowWindow(m_impl->hwnd, SW_SHOWMAXIMIZED);
	} else {
		util::CenterWindow(GetDesktopWindow(), m_impl->hwnd);
		::ShowWindow(m_impl->hwnd, SW_SHOW);
	}

	if(m_impl->doc->GetReadOnly() )
	{
		::SendMessage(m_impl->hwndEdit, EM_SETREADONLY, 1, 0);
//		DisableModify();
	}
}


Editor::~Editor()
{
	delete m_impl;
}


bool Editor::IsModified() const
{
	return (0 != ::SendMessage(m_impl->hwndEdit, EM_GETMODIFY, 0, 0));
}


HWND Editor::GetHWND() const
{
	return m_impl->hwnd;
}


void Editor::SetPosition(unsigned x, unsigned y)
{
	::SetWindowPos(m_impl->hwnd, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}


void Editor::SetSize(unsigned w, unsigned h)
{
	::SetWindowPos(m_impl->hwnd, 0, 0, 0, w, h, SWP_NOZORDER | SWP_NOREPOSITION);
}


size_t Editor::GetTextLength()
{
	return static_cast<size_t>( ::SendMessage(m_impl->hwndEdit, WM_GETTEXTLENGTH, 0, 0));
}


struct StreamOutStruct
{
	StreamOutStruct(Plaintext& pt) : str(pt) {}
	Plaintext& str;
private:
	StreamOutStruct& operator= (const StreamOutStruct&);
};
static DWORD WINAPI streamOutString(DWORD_PTR cookie, LPBYTE buf, LONG cb, LONG* pcb)
{
	StreamOutStruct *stream = reinterpret_cast<StreamOutStruct*>( cookie );

	stream->str.Append(buf, cb);

	*pcb = cb;
	return 0;
}
void Editor::GetText(Plaintext& text)
{
	size_t len = GetTextLength();

	Plaintext plain( len*2 );	// *2 because of unicode

	StreamOutStruct	stream(plain);
	EDITSTREAM	es;
	es.dwCookie = reinterpret_cast<DWORD_PTR>(&stream);
	es.dwError = 0;
	es.pfnCallback = streamOutString;

	::SendMessage(m_impl->hwndEdit, EM_STREAMOUT, SF_TEXT|SF_UNICODE, reinterpret_cast<LPARAM>( &es ) );

	util::WideToNarrow(text, plain);
}


struct StreamInStruct
{
	StreamInStruct(const Plaintext& pt) : str(pt), pos(0) {}
	const Plaintext& str;
	size_t pos; // current read position
private:
	StreamInStruct operator= (const StreamInStruct&);
};
static DWORD WINAPI streamInString(DWORD_PTR cookie, LPBYTE buf, LONG cb, LONG* pcb)
{
	StreamInStruct *stream = reinterpret_cast<StreamInStruct*>( cookie );

	assert(cb >= 0);
	size_t byteCount = cb;
	size_t bytesRemaining = stream->str.size() - stream->pos;
	size_t bytesToRead = (byteCount > bytesRemaining) ? bytesRemaining : cb;

	if(bytesToRead)
	{
		memcpy(buf, static_cast<char*>( stream->str.data() ) + stream->pos, bytesToRead);
		stream->pos += bytesToRead;
	}
	assert(bytesToRead <= std::numeric_limits<LONG>::max());
	*pcb = static_cast<LONG>(bytesToRead);

	return 0;
}
void Editor::SetText(const Plaintext& text)
{
	Plaintext	plain(text.size() * 2);		// *2 because of unicode output

	StreamInStruct	stream(plain);

	EDITSTREAM	es;
	es.dwCookie = reinterpret_cast<DWORD_PTR>(&stream);
	es.dwError = 0;
	es.pfnCallback = streamInString;

	util::NarrowToWide(plain, text);

	::SendMessage(m_impl->hwndEdit, EM_STREAMIN, SF_TEXT|SF_UNICODE, reinterpret_cast<LPARAM>( &es ) );
}


LRESULT Editor::Dispatch(HWND hwnd, unsigned msg, WPARAM wParam, LPARAM lParam)
{
	if( FindReplace::Instance()->ProcessMessage(msg) )
	{
		OnFindReplace( hwnd, *(reinterpret_cast<FINDREPLACE*>( lParam )));
		return 0;
	}

	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_ACTIVATE, OnActivate);
	HANDLE_MSG (hwnd, WM_CLOSE, OnClose);
	HANDLE_MSG (hwnd, WM_COMMAND, OnCommand);
	HANDLE_MSG (hwnd, WM_CREATE, OnCreate);
	HANDLE_MSG (hwnd, WM_DESTROY, OnDestroy);
	HANDLE_MSG (hwnd, WM_DROPFILES, OnDropFiles);
	HANDLE_MSG (hwnd, WM_INITMENUPOPUP, OnInitMenuPopup);
	HANDLE_MSG (hwnd, WM_NOTIFY, OnNotify);
	HANDLE_MSG (hwnd, WM_SIZE, OnSize);

	default: return ::DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// private member functions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Editor::setFont(HFONT font)
{
	::SendMessage(m_impl->hwndEdit, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
	if(m_impl->font)
		::DeleteObject(m_impl->font);
	m_impl->font = font;
	//FIXME: when changing font, auto-url links are lost... and unfortunately,
	//flipping EM_AUTOURLDETECT doesn't seem to be a working hack :(
}


void Editor::loadFontSettings()
{
	xstring fontface;
	unsigned	fontsize;

	font::font_load_settings(fontface, fontsize);
	setFont(font::font_create(fontface.c_str(), fontsize, 0, 0));
}


void Editor::PrepareFileMenu(HMENU menu)
{
	// if RichEdit control is empty, disable Save/SaveAs/Export, otherwise
	// enable them.
	UINT flags1, flags2;

	flags1 = (GetTextLength() == 0) ?
		(MF_BYCOMMAND | MF_GRAYED) : (MF_BYCOMMAND | MF_ENABLED);

	EnableMenuItem(menu, ID_FILE_SAVE, flags1);
	EnableMenuItem(menu, ID_FILE_SAVE_AS, flags1);
	EnableMenuItem(menu, ID_FILE_EXPORT, flags1);

	// if note is read-only, disable import, password change, make readonly
	flags2 = ( m_impl->doc->GetReadOnly() ) ?
		(MF_BYCOMMAND | MF_GRAYED) : (MF_BYCOMMAND | MF_ENABLED);

	EnableMenuItem(menu, ID_FILE_IMPORT, flags2);
	EnableMenuItem(menu, ID_FILE_SETKEY, flags2);
	EnableMenuItem(menu, ID_FILE_SAVE_AS, flags2);
	EnableMenuItem(menu, ID_FILE_SAVEREADONLY, flags2);

	// File->Save is special, it must be grayed out if _either_ of read-only
	// or empty text conditions are met. Binary operators are nice, this
	// whole function is entirely branchless now :-)
	flags2 |= (flags1 & MF_GRAYED);		// has to be MF_GRAYED, MF_EMABLED is a dummy. 
	EnableMenuItem(menu, ID_FILE_SAVE, flags2);
}


void Editor::PrepareEditMenu(HMENU menu)
{
	CHARRANGE	range;
	bool isEnabled;

	// ask RichEdit whether we can PASTE, UNDO, REDO, enable/disable items accordingly
	isEnabled = 0 != ::SendMessage(m_impl->hwndEdit, EM_CANPASTE, CF_TEXT, 0);
	::EnableMenuItem(menu, ID_EDIT_PASTE, isEnabled ? MF_ENABLED : MF_GRAYED);
	isEnabled = 0 != ::SendMessage(m_impl->hwndEdit, EM_CANUNDO, 0, 0);
	::EnableMenuItem(menu, ID_EDIT_UNDO, isEnabled ? MF_ENABLED : MF_GRAYED);
	isEnabled = 0 != ::SendMessage(m_impl->hwndEdit, EM_CANREDO, 0, 0);
	::EnableMenuItem(menu, ID_EDIT_REDO, isEnabled ? MF_ENABLED : MF_GRAYED);

	// get selection range, enable COPY if not empty
	::SendMessage(m_impl->hwndEdit, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>( &range) );
	isEnabled = (range.cpMin != range.cpMax);
	::EnableMenuItem(menu, ID_EDIT_COPY, isEnabled ? MF_ENABLED : MF_GRAYED);

	// turn off CUT and DELETE if control is read-only
	if(::GetWindowLong(m_impl->hwndEdit, GWL_STYLE) & ES_READONLY)
		isEnabled = false;
	::EnableMenuItem(menu, ID_EDIT_CUT, isEnabled ? MF_ENABLED : MF_GRAYED);
	::EnableMenuItem(menu, ID_EDIT_DELETE, isEnabled ? MF_ENABLED : MF_GRAYED);
}


static DWORD WINAPI streamInText(DWORD_PTR cookie, LPBYTE buf, LONG cb, LONG* pcb)
{
	return ::ReadFile(reinterpret_cast<HANDLE>(cookie), buf, cb, reinterpret_cast<LPDWORD>(pcb), 0) ? 0 : -1;
}


bool Editor::DoImport(const xstring& filename)
{
	HANDLE		hf = ::CreateFile(filename.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if(INVALID_HANDLE_VALUE == hf)
		return false;

	EDITSTREAM	es;
	es.dwCookie = reinterpret_cast<DWORD_PTR>(hf);
	es.dwError = 0;
	es.pfnCallback = streamInText;

	::SendMessage(m_impl->hwndEdit, EM_STREAMIN, SF_TEXT, reinterpret_cast<LPARAM>( &es ) );

	::CloseHandle(hf);

	return true;
}


DWORD WINAPI streamOutText(DWORD_PTR cookie, LPBYTE buf, LONG cb, LONG* pcb)
{
	return ::WriteFile(reinterpret_cast<HANDLE>(cookie), buf, cb, reinterpret_cast<LPDWORD>(pcb), 0) ? 0 : -1;
}


bool Editor::DoExport(const xstring& filename)
{
	HANDLE		hf = ::CreateFile(filename.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	if(INVALID_HANDLE_VALUE == hf)
		return false;

	EDITSTREAM	es;
	es.dwCookie = reinterpret_cast<DWORD_PTR>(hf);
	es.dwError = 0;
	es.pfnCallback = streamOutText;

	::SendMessage(m_impl->hwndEdit, EM_STREAMOUT, SF_TEXT, reinterpret_cast<LPARAM>( &es ) );

	::CloseHandle(hf);
	return true;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static TCHAR const* propName = _T("fSekrit::dispatch");

	switch(msg)
	{
	case WM_CREATE:
		::SetProp(hwnd, propName, reinterpret_cast<CREATESTRUCT*>(lp)->lpCreateParams);
		// intentional fallthrough!

	default:
		HANDLE prop = ::GetProp(hwnd, propName);
		if(prop)
		{
			return reinterpret_cast<Editor*>(prop)->Dispatch(hwnd, msg,
				static_cast<unsigned>(wp), static_cast<unsigned>(lp));
		} else
		{
			return ::DefWindowProc(hwnd, msg, wp, lp);
		}
	}
}


void Editor::OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
{
	UNUSED(hwnd);
	UNUSED(state);
	UNUSED(hwndActDeact);
	UNUSED(fMinimized);

	::SetFocus(m_impl->hwndEdit);
}


void Editor::OnDropFiles(HWND hwnd, HDROP hDropInfo)
{
	UNUSED(hwnd);
	// For handling multiple files, we can query the count of drops this way:
	// DragQueryFile(hDropInfo, 0xFFFFFFFF, 0, 0);

	if(!CheckAndHandleModified())
		return;

	TCHAR	dropPath[MAX_PATH];
	::DragQueryFile(hDropInfo, 0, dropPath, lengthof(dropPath));
	DoImport(dropPath);
}


void Editor::OnClose(HWND hwnd)
{
	if(CheckAndHandleModified())
		::DestroyWindow(hwnd);
}


void Editor::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	UNUSED(hwnd);
	UNUSED(hwndCtl);
	UNUSED(codeNotify);

	switch(id)
	{
	case ID_FILE_NEW:		handle_new();		break;
	case ID_FILE_SAVE:		handle_save();		break;
	case ID_FILE_SAVE_AS:	handle_saveas(false);	break;
	case ID_FILE_IMPORT:	handle_import();	break;
	case ID_FILE_EXPORT:	handle_export();	break;
	case ID_FILE_SETKEY:	handle_setkey();	break;
	case ID_FILE_SAVEREADONLY:	handle_saveas(true);	break;
	case ID_FILE_EXIT:		handle_exit();		break;

	case ID_EDIT_UNDO:		SendMessage(m_impl->hwndEdit, EM_UNDO, 0, 0);	break;
	case ID_EDIT_REDO:		SendMessage(m_impl->hwndEdit, EM_REDO, 0, 0);	break;
	case ID_EDIT_CUT:		SendMessage(m_impl->hwndEdit, WM_CUT, 0, 0);	break;
	case ID_EDIT_COPY:		SendMessage(m_impl->hwndEdit, WM_COPY, 0, 0);	break;
	case ID_EDIT_PASTE:		SendMessage(m_impl->hwndEdit, WM_PASTE, 0, 0);	break;
	case ID_EDIT_DELETE:	SendMessage(m_impl->hwndEdit, WM_CLEAR, 0, 0);	break;
	case ID_EDIT_FONT:		handle_font();		break;

	case ID_EDIT_FIND:		handle_find();		break;
	case ID_EDIT_REPEAT:	handle_findnext();	break;
//	case ID_EDIT_REPLACE:	handle_replace();	break;

	case ID_HELP_ABOUT:		handle_about();		break;


	case ID_EDIT_SELECT_ALL:
		::SendMessage(m_impl->hwndEdit, EM_SETSEL, 0, -1);
		break;
	}
}


void Editor::handle_font()
{
	xstring font_face;
	unsigned font_size;

	if(font::do_choose_font(m_impl->hwnd, font_face, font_size))
	{
		font::font_save_settings(font_face, font_size);
		setFont(font::font_create(font_face.c_str(), font_size, 0));
	}
}


void Editor::handle_find()
{
	CHARRANGE	pos;

	::SendMessage(m_impl->hwndEdit, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&pos));
	FindReplace::Instance()->SetOwnerWindow(m_impl->hwnd, m_impl->hwndEdit);
	FindReplace::Instance()->StartFind(pos.cpMin, -1);
}


void Editor::handle_findnext()
{
	FindReplace &finder = *FindReplace::Instance();

	if(-1 == finder.NextFind() )
	{
		::MessageBox(0, strNoMoreMatches, strAppName, MB_OK);
		::SendMessage(m_impl->hwndEdit, EM_SETSEL, 0, 0);
	} else
	{
		::SendMessage(m_impl->hwndEdit, EM_SETSEL, finder.GetMatchStart(), finder.GetMatchEnd());
	}
}


void Editor::handle_replace()
{
	util::error_box(strNotImplemented);

	/*
	CHARRANGE	pos;

	SendMessage(m_impl->hwndEdit, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&pos));
	FindReplace::Instance()->SetOwnerWindow(m_impl->hwnd, m_impl->hwndEdit);
	FindReplace::Instance()->StartReplace(pos.cpMin, -1);
	*/
}


void Editor::handle_about()
{
	DlgAbout::invoke(m_impl->hwnd);	
}


BOOL Editor::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	UNUSED(lpCreateStruct);

	RECT	rc;

	m_impl->hwnd = hwnd;
	UpdateTitle();

	// set application/main window icon
	HICON	icon = (HICON) ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	::SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM) icon);

	// create an edit control and set it's size to the entire client area	
	::GetClientRect((HWND)hwnd, &rc);
	m_impl->hwndEdit = ::CreateWindow(RICHEDIT_CLASS, 0,
		WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | 
		WS_VSCROLL | ES_NOHIDESEL | ES_WANTRETURN,
		0, 0, rc.right, rc.bottom,
		(HWND) hwnd, 0, GetModuleHandle(0), 0);

	DWORD mask = ::SendMessage(m_impl->hwndEdit, EM_GETEVENTMASK, 0, 0);
	::SendMessage(m_impl->hwndEdit, EM_SETEVENTMASK, 0, mask | ENM_LINK | ENM_MOUSEEVENTS);
	::SendMessage(m_impl->hwndEdit, EM_AUTOURLDETECT, TRUE, 0);

//	::SendMessage(m_impl->hwndEdit, EM_SETWORDWRAPMODE, WBF_WORDBREAK, 0);
	::SendMessage(m_impl->hwndEdit, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
	::SendMessage(m_impl->hwndEdit, EM_EXLIMITTEXT, 0, MAX_EDIT_SIZE);

	loadFontSettings();

	::RedrawWindow(m_impl->hwndEdit, 0, 0, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW);

	return TRUE;
}


void Editor::OnDestroy(HWND hwnd)
{
	UNUSED(hwnd);

	::PostQuitMessage(0);
}


void Editor::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	UNUSED(hwnd);
	UNUSED(state);

	// resize edit control to entire client area
	::MoveWindow(m_impl->hwndEdit, 0, 0, cx, cy, TRUE);
}


void Editor::OnFindReplace(HWND hwnd, const FINDREPLACE &fr)
{
	UNUSED(hwnd);

    if (fr.Flags & FR_FINDNEXT)
		handle_findnext();
}


void Editor::handle_richpopup(int x, int y)
{
	POINT	ptPopupPos;
	HMENU	hMenu, hPopup;
	int		choice;

	ptPopupPos.x = x;
	ptPopupPos.y = y;
	::ClientToScreen(m_impl->hwndEdit, &ptPopupPos);
	
	hMenu = ::LoadMenu(0, MAKEINTRESOURCE(IDR_REPOPUP));
	hPopup = ::GetSubMenu(hMenu, 0);

	PrepareEditMenu(hPopup);

	choice = ::TrackPopupMenu(hPopup,
		TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD | TPM_NONOTIFY,
		ptPopupPos.x, ptPopupPos.y, 0, m_impl->hwndEdit, 0);

	::DestroyMenu(hMenu);

	if(choice)
		::SendMessage(m_impl->hwnd, WM_COMMAND, choice, 0);
}


void Editor::handle_richlink(const ENLINK& eli)
{
	std::vector<TCHAR> buf;
	buf.resize(eli.chrg.cpMax - eli.chrg.cpMin);

	TEXTRANGE tr;
	tr.chrg = eli.chrg;
	tr.lpstrText = &buf[0];

	::SendMessage(m_impl->hwndEdit, EM_GETTEXTRANGE, 0,
		reinterpret_cast<LPARAM>(&tr));
	::ShellExecute(m_impl->hwnd, _T("open"), &buf[0], 0, 0, SW_SHOWDEFAULT);
}


LRESULT Editor::OnNotify(HWND hwnd, int ctrl, LPNMHDR nmh)
{
	UNUSED(hwnd);
	UNUSED(ctrl);

	switch(nmh->code)
	{
	case EN_MSGFILTER:
		{
		MSGFILTER& mf = *(reinterpret_cast<MSGFILTER*> (nmh));
		if(mf.msg == WM_RBUTTONDOWN)
		{
			handle_richpopup(LOWORD(mf.lParam), HIWORD(mf.lParam));
			return 1;
		}
		return 0;
		}

	case EN_LINK:
		{
		ENLINK& eli = *(reinterpret_cast<ENLINK*> (nmh));
		if(eli.msg == WM_LBUTTONDOWN)
		{
			handle_richlink(eli);
			return 1;
		}
		return 0;
		}

	default:
		return 0;
	}
}


void Editor::OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
	UNUSED(hwnd);
	UNUSED(fSystemMenu);

	switch(item)
	{
	case 0: PrepareFileMenu(hMenu); break;
	case 1: PrepareEditMenu(hMenu); break;
	}
}


void Editor::UpdateTitle(const TCHAR* title)
{
	xstring temp( strAppName );

	temp.append(_T(" version "));
	temp.append(strVersion);
	temp.append(_T(": "));

	if(!title)
		temp.append(m_impl->doc->GetName());
	else
		temp.append(title);

	if( m_impl->doc->GetReadOnly() )
		temp.append(_T(" [read-only]"));

	::SetWindowText(m_impl->hwnd, temp.c_str());
}


bool Editor::CheckAndHandleModified()
{
	if(!IsModified())
		return true;

	int result =
		::MessageBox(m_impl->hwnd, strSavePrompt, strAppName, MB_YESNOCANCEL);

	switch(result)
	{
	case IDYES:		return handle_save();
	case IDNO:		return true;
	case IDCANCEL:	return false;
	default:		return false;
	}
}


void Editor::handle_new()
{
	if(!CheckAndHandleModified())
		return;

	UpdateTitle(strNewDocument);
//	EnableModify();
	SetText(0);
	m_impl->doc->ClearKey();
	m_impl->doc->SetReadOnly(false);
	m_impl->doc->Clear();
}


bool Editor::handle_save()
{
	if(0 == ::GetWindowTextLength(m_impl->hwndEdit))
	{
		util::error_box(strNoSaveEmpty);
		return false;
	}

	RECT rc;
	::GetWindowRect(m_impl->hwnd, &rc);
	g_ugly_x = rc.left;
	g_ugly_y = rc.top;
	g_ugly_w = rc.right - rc.left;
	g_ugly_h = rc.bottom - rc.top;
	g_ugly_maximized = ( 0 != IsMaximized(m_impl->hwnd) );

	Plaintext plaintext;

	if( m_impl->doc->GetName().empty() )
	{
		// we don't have a filename, do save-as
		return handle_saveas(false);
	} else
	{
		// we have a filename
		GetText(plaintext);
		m_impl->doc->SetText(plaintext);

		if(m_impl->doc->Save())
			::SendMessage(m_impl->hwndEdit, EM_SETMODIFY, 0, 0);
		else
		{
			util::error_box(strErrorSave);
			return false;
		}

		return true;
	}
}


bool Editor::handle_saveas(bool readonly)
{
	if(0 == GetWindowTextLength(m_impl->hwndEdit))
	{
		util::error_box(strNoSaveEmpty);
		return false;
	}

	RECT rc;
	::GetWindowRect(m_impl->hwnd, &rc);
	g_ugly_x = rc.left;
	g_ugly_y = rc.top;
	g_ugly_w = rc.right - rc.left;
	g_ugly_h = rc.bottom - rc.top;

	xstring filename;

	if(util::get_save_filename(m_impl->hwnd, filename, _T("*.exe")))
	{
		if(util::file_exists(filename))
		{
			int result =
				::MessageBox(m_impl->hwnd, strFileAlreadyExists, strAppName, MB_YESNO);

			if(IDNO == result)
				return false;
		}

		if(!m_impl->doc->IsKeySet())
		{
			Plaintext passphrase;
			if(!DlgPass2::invoke(passphrase))
				return false;
			m_impl->doc->SetKey(passphrase);
		}

		Plaintext plaintext;
		GetText(plaintext);
		m_impl->doc->SetText(plaintext);

		m_impl->doc->SetReadOnly(readonly);
		if(m_impl->doc->SaveAs(filename))
		{
			::SendMessage(m_impl->hwndEdit, EM_SETREADONLY, readonly, 0);
			::SendMessage(m_impl->hwndEdit, EM_SETMODIFY, 0, 0);
			UpdateTitle();
			return true;
		} else {
			util::error_box(strErrorSave);
			return false;
		}
	}

	return false;
}


void Editor::handle_import()
{
	if( !CheckAndHandleModified() )
		return;


	xstring filename;
	if( !util::get_open_filename(m_impl->hwnd, filename, _T("Text Files (*.txt)\0*.txt")) )
		return;

	if(!DoImport(filename))
	{
		util::error_box(strErrorLoad);
		return;
	}
}


void Editor::handle_export()
{
	xstring filename;

	if( !util::get_save_filename(m_impl->hwnd, filename, _T("*.txt")) )
		return;

	if(!DoExport(filename))
	{
		util::error_box(strErrorSave);
		return;
	}
}


void Editor::handle_setkey()
{
	Plaintext passphrase;

	if(DlgPass2::invoke(passphrase))
	{
		m_impl->doc->ChangeKey(passphrase);
		::SendMessage(m_impl->hwndEdit, EM_SETMODIFY, 1, 0);
	}
}


void Editor::handle_exit()
{
	::SendMessage(m_impl->hwnd, WM_CLOSE, 0, 0);
}


} // editor namespace
