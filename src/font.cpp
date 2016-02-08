#include "precompiled.h"
#include "globals.h"

#include "font.h"

namespace font
{

static const unsigned	default_fontsize = 10;
static const TCHAR*	default_fontface = _T("Courier New");

static DWORD getHeightForPointSize(HWND hwnd, unsigned size)
{
	HDC		hdc = ::GetDC(hwnd);
	DWORD	height = -::MulDiv(size, ::GetDeviceCaps(hdc, LOGPIXELSY), 72);
	::ReleaseDC(0, hdc);

	return height;
}

HFONT font_create(const TCHAR* a_type, const unsigned a_size, const unsigned a_style, const HWND a_hwnd)
{
	HFONT	font;
	DWORD	weight = (a_style & FS_BOLD) ? FW_BOLD : FW_NORMAL;
	DWORD	height = getHeightForPointSize(a_hwnd, a_size);

	font = ::CreateFont(height, 0,
		0, 0, weight, (a_style & FS_ITALIC), (a_style & FS_UNDERLINE), (a_style & FS_STRIKEOUT),
		DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, a_type);

	return font;
}


bool do_choose_font(HWND hwnd, xstring& face, unsigned& size)
{
	CHOOSEFONT cf;
	LOGFONT lf;

	//TODO: fill font structure from registry data
	memset(&lf, 0, sizeof(lf));
	memset(&cf, 0, sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = hwnd;
	cf.lpLogFont = &lf;
	cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;

	xstring fontface;
	unsigned fontsize;
	font::font_load_settings(fontface, fontsize);

	lf.lfHeight = getHeightForPointSize(hwnd, fontsize);
//	_tcscpy_s(lf.lfFaceName, sizeof(lf.lfFaceName), fontface.c_str());
	strcpy(lf.lfFaceName, fontface.c_str());

	if(::ChooseFont(&cf))
	{
		face.assign(cf.lpLogFont->lfFaceName);
		size = cf.iPointSize/10;
		return true;
	} else
		return false;
}


void font_load_settings(xstring& face, unsigned& size)
{
	TCHAR faceTemp[32];
	HKEY hkey;
	if( ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, strRegConfigPath, 0, KEY_READ, &hkey) )
	{
		DWORD dummy;

		dummy = sizeof(faceTemp);
		if( ERROR_SUCCESS != ::RegQueryValueEx(hkey, _T("fontface"), 0, 0,
			reinterpret_cast<LPBYTE>( &faceTemp ), &dummy) )
			face.assign(default_fontface);
		else
			face.assign(faceTemp);
//			_tcscpy_s(fontface, lengthof(fontface), default_fontface);

		dummy = sizeof(size);
		if( ERROR_SUCCESS != ::RegQueryValueEx(hkey, _T("fontsize"), 0, 0,
			reinterpret_cast<LPBYTE>( &size ), &dummy) )
			size = default_fontsize;
		::RegCloseKey(hkey);
	} else
	{
		face.assign(default_fontface);
		size = default_fontsize;
	}
}


void font_save_settings(const xstring& face, unsigned size)
{
	HKEY hkey;
	if( ERROR_SUCCESS == ::RegCreateKeyEx(HKEY_CURRENT_USER,
		strRegConfigPath, 0, 0, 0, KEY_WRITE, 0, &hkey, 0) )
	{
		DWORD dummy;

		dummy = face.size() * sizeof(face[0]);
		::RegSetValueEx(hkey, _T("fontface"), 0, REG_SZ,
			reinterpret_cast<LPBYTE>( const_cast<TCHAR*>(face.c_str())), dummy);
		dummy = 4;
		::RegSetValueEx(hkey, _T("fontsize"), 0, REG_DWORD,
			reinterpret_cast<LPBYTE>( &size), dummy);

		::RegCloseKey(hkey);
	}
}

}
