#pragma once
#ifndef id2C5A2BEF_E5AD_400C_AA3A7C6A512B4A02
#define id2C5A2BEF_E5AD_400C_AA3A7C6A512B4A02

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

// various strings
extern const TCHAR* strVersion;
extern const TCHAR* strAppName;
extern const TCHAR* strSavePrompt;
extern const TCHAR* strInvalidPassphrase;
extern const TCHAR* strErrorLoad;
extern const TCHAR* strErrorSave;
extern const TCHAR* strNotImplemented;
extern const TCHAR* strStayTuned;
extern const TCHAR* strMalformedArg;
extern const TCHAR* strErrorErase;
extern const TCHAR* strErrorCopy;
extern const TCHAR* strErrorLaunch;
extern const TCHAR* strNewDocument;
extern const TCHAR* strFileAlreadyExists;
extern const TCHAR* strRichEditDLL;
extern const TCHAR* strErrorLoadRichEd;
extern const TCHAR* strErrorReadOnly;
extern const TCHAR* strNoMoreMatches;
extern const TCHAR* strErrorBackwardsCompatible;
extern const TCHAR* strRegConfigPath;
extern const TCHAR* strNoSaveEmpty;

extern bool			g_editorMode;					// launched with -edit argument?
extern xstring		g_documentFilePath;				// path/filename of the document being edited
extern xstring		g_editorTempPath;				// path where temporary editor executable is stored.
extern TCHAR		g_moduleName[MAX_PATH];			// path/filename of the running .exe
extern unsigned		g_findReplaceMsg;
extern HINSTANCE	g_hInstance;					// ...

// used when saving... I really need a cleaner interface for this, but I also
// really need to ship version 1.0 before deadline. Sigh :P
extern unsigned g_ugly_w, g_ugly_h, g_ugly_x, g_ugly_y;
extern bool		g_ugly_maximized;					// [1.2] - something needs a cleanup...

#endif // header
