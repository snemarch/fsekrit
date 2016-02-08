#pragma once
#ifndef idE3159CE4_B4A5_48FE_9A7D9BD86143DC54
#define idE3159CE4_B4A5_48FE_9A7D9BD86143DC54

#define WINVER			0x0400
#define _WIN32_WINDOWS	0x0400
#define _WIN32_WINNT	0x0400

//
// precompiled header
//
#define UNUSED(x) (void) x

// WINDOWS headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <RichEdit.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <tchar.h>
#include "resource.h"

#include <windowsx.h>		// must come after all other windows headers!

#undef min
#undef max

// STANDARD headers
//C++
#include <algorithm>
#include <exception>
#include <limits>
#include <memory>
#include <string>
#include <vector>
//C
#include <assert.h>
#include <memory.h>
#include <stdlib.h>

// application headers etc
#ifdef _UNICODE
typedef std::wstring xstring;
#else
typedef std::string xstring;
#endif

#pragma hdrstop

#endif // header

