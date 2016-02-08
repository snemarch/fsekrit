#include "precompiled.h"
#include "util.h"
#include "file.h"

namespace util
{

size_t roundup(size_t val, size_t amt)
{
	size_t mod;

	if(val == 0) return amt;
	
	mod = val % amt;
	if(mod == 0) return val;

	return val + amt - mod;
}


bool get_open_filename(HWND hwnd, xstring &name, const xstring &filters)
{
	TCHAR			buf[MAX_PATH];
	TCHAR			cwd[MAX_PATH];
	OPENFILENAME	ofn;

	memset(&ofn, 0, sizeof(ofn));
	GetCurrentDirectory(lengthof(cwd), cwd);

	std::vector<TCHAR> filt(filters.size() + 1);
	memcpy(&filt[0], filters.c_str(), filters.size() * sizeof(TCHAR) );
	filt.push_back(0);

	buf[0] = 0;
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = buf;
	ofn.nMaxFile = lengthof(buf);
	ofn.nFilterIndex = 1;
	ofn.lpstrFilter = &filt[0];
	ofn.hwndOwner = hwnd;
	ofn.lpstrInitialDir = cwd;
	ofn.Flags = OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if(!GetOpenFileName(&ofn))
		return false;

    name.assign(buf);

	return true;
}


bool get_save_filename(HWND hwnd, xstring &name, const xstring &filters)
{
	TCHAR			buf[MAX_PATH];
	TCHAR			cwd[MAX_PATH];
	OPENFILENAME	ofn;

	memset(&ofn, 0, sizeof(ofn));
	GetCurrentDirectory(lengthof(cwd), cwd);

	std::vector<TCHAR> filt(filters.size() + 1);
	memcpy(&filt[0], filters.c_str(), filters.size() * sizeof(TCHAR));
	filt.push_back(0);

	buf[0] = 0;
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = buf;
	ofn.nMaxFile = lengthof(buf);
	ofn.nFilterIndex = 1;
	ofn.lpstrFilter = &filt[0];
	ofn.hwndOwner = hwnd;
	ofn.lpstrInitialDir = cwd;
	ofn.Flags = OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

	if(!GetSaveFileName(&ofn))
		return false;

	// yes, firstFilter is a hack, but currently get_save_filename is only ever
	// called with one filter. Should probably be fixed up to take an array-of-pointers
	// with filters, without "*." part, and code above should then construct win32
	// filter list from that.
	xstring firstFilter(filters.begin() + 1, filters.end() );

	name.assign(buf);
	if(!ofn.nFileExtension)
		name.append(firstFilter);

	return true;
}

/**
* Returns fully qualified filename of a temporary file.
* @param path path to create the tempfile in
* @param prefix prefix before random number
* @param suffix .suffix after random number
* @return path to temporary file
*/
xstring get_temp_name(const xstring& path, const TCHAR* prefix, const TCHAR* suffix)
{
	TCHAR buf[MAX_PATH];

//	_stprintf_s(buf, lengthof(buf), _T("%s%s-%04X.%s"), path.c_str(), prefix, rand(), suffix);
	sprintf(buf, _T("%s%s-%04X.%s"), path.c_str(), prefix, rand(), suffix);

	return xstring(buf);
}

bool file_exists(const xstring &name)
{
	return (INVALID_FILE_ATTRIBUTES != GetFileAttributes(name.c_str()));
}


bool file_copy(const xstring &target, const xstring &source)
{
	if (0 == CopyFile(source.c_str(), target.c_str(), FALSE) )
		return false;

	// CopyFile() also copies file attributes, this can be bad in the case
	// where a source file has +Readonly attribute set. So, clear it.

	DWORD	fAttr = GetFileAttributes(target.c_str());
	fAttr &= ~FILE_ATTRIBUTE_READONLY;
	SetFileAttributes(target.c_str(), fAttr);

	return true;
}


bool file_copy_range(const xstring &target, const xstring &source, size_t ofs, size_t len)
{
	file::File	ftarget(target.c_str(), true, true);
	file::File	fsource(source.c_str(), false, false);

	fsource.Seek(ofs);

	char buf[4096];
	while( len >= sizeof(buf) )
	{
		fsource.Read(buf, sizeof(buf));
		ftarget.Write(buf, sizeof(buf));
		len -= sizeof(buf);
	}
	if(len)
	{
		fsource.Read(buf, len);
		ftarget.Write(buf, len);
	}

	return true;
}


void error_box(const TCHAR *message)
{
	MessageBox(0, message, _T("fSekrit::error"), MB_OK | MB_ICONEXCLAMATION);
}


bool launch(const xstring &program, const xstring &parms, bool wait)
{
	PROCESS_INFORMATION	pi;
	STARTUPINFO			si;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	xstring arg = xstring(_T("\"")) + program + xstring(_T("\" ")) + parms;

	if(0 == CreateProcess(0, const_cast<PTCHAR>( arg.c_str() ), 0, 0, 0, 0, 0, 0, &si, &pi))
		return false;

	if(wait)
		WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return true;
}

/**
* Returns base path from a full path - "c:\foo\myfile.ext" returns "c:\foo".
* @param path path to extract base path from.
* @return base path.
*/
xstring get_base_path(const xstring& path)
{
	xstring::size_type	pos;

	pos = path.find_last_of('\\');
	if(pos != xstring::npos)
	{
		return xstring(path, 0, pos+1);
	} else 
	{
		return path;
	}

}


/**
* Returns base filename from a full path - "c:\foo\myfile.ext" returns "myfile.ext".
* @param path path to extract base name from.
* @return base name.
*/
xstring get_base_name(const xstring& path)
{
	xstring::size_type	pos;

	pos = path.find_last_of('\\');
	if(pos != xstring::npos)
	{
		return xstring(path, pos+1, path.length());
	} else 
	{
		return path;
	}
}


std::string bin2hex(const std::vector<char> &data)
{
	static char hextable[] = "0123456789ABCDEF";
	std::string temp;

	for(std::vector<char>::const_iterator i = data.begin();
		i != data.end(); ++i)
	{
		const register unsigned char ch = *i;
		temp.push_back( hextable[ch >> 4] );
		temp.push_back( hextable[ch & 0xF]);
	}

	return temp;
}

static unsigned char hexbyte(unsigned char input)
{
	if(input >= '0' && input <= '9')
		return (input - '0');

	if(input >= 'a' && input <= 'f')
		return (input - 'a') + 10;

	if(input >= 'A' && input <= 'F')
		return (input - 'A') + 10;

	return 0;
}

std::vector<char> hex2bin(const std::string &data)
{
	std::vector<char> temp;

	bool gotOne = false;
	unsigned char blob = 0;

	for(std::string::const_iterator i = data.begin();
		i != data.end(); ++i)
	{
		const register unsigned char ch = *i;

		blob = (blob << 4) | hexbyte(ch);
		if(gotOne)
		{
			temp.push_back(blob);
			blob = 0;
			gotOne = false;
		} else gotOne = true;
	}

	return temp;
}


void CenterWindow(HWND centerOnhWnd, HWND hWnd)
{
	RECT			rect1;
	RECT			rect2;
	WINDOWPLACEMENT	wp;

	memset(&wp, 0, sizeof(WINDOWPLACEMENT) );
	wp.length = sizeof(WINDOWPLACEMENT);

	GetWindowRect(centerOnhWnd,&rect1);
	GetWindowRect(hWnd,&rect2);
	
	long x = (rect2.right - rect2.left);
	long y = (rect2.bottom - rect2.top);

	MoveWindow(hWnd,
		( (rect1.right - rect1.left) / 2) - (x / 2) + wp.rcNormalPosition.left,
		( (rect1.bottom - rect1.top) / 2) - (y / 2) + wp.rcNormalPosition.top,
		x, y, true);
}


void fill_random(void *buffer, size_t length)
{
	unsigned char *p = static_cast<unsigned char*>( buffer );

	while(length--)
		*p++ = rand() % 0xFF;
}


void WideToNarrow(Plaintext& out, const Plaintext& in)
{
	assert(in.size() <= std::numeric_limits<size_t>::max());

	size_t newlen = WideCharToMultiByte(CP_UTF8, 0,
		reinterpret_cast<LPCWSTR>(in.data()), static_cast<int>( in.size()/2 ), 0, 0, 0, 0);

	out.resize(newlen);
	WideCharToMultiByte(CP_UTF8, 0,
		reinterpret_cast<LPCWSTR>(in.data()), static_cast<int>( in.size()/2 ),
		reinterpret_cast<LPSTR>(out.data()), static_cast<int>( out.size() ), 0, 0);
}


void NarrowToWide(Plaintext& out, const Plaintext& in)
{
	assert(in.size() <= std::numeric_limits<size_t>::max());

	size_t newlen = MultiByteToWideChar(CP_UTF8, 0,
		reinterpret_cast<LPSTR>(in.data()), static_cast<int>( in.size() ), 0, 0);

	out.resize(newlen*2);
	MultiByteToWideChar(CP_UTF8, 0,
		reinterpret_cast<LPSTR>(in.data()), static_cast<int>( in.size() ),
		reinterpret_cast<LPWSTR>(out.data()), static_cast<int>( newlen) );
}

} // util namespace
