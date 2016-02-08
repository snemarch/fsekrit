#include "precompiled.h"
#include "globals.h"

bool		g_editorMode = false;
xstring		g_documentFilePath;
xstring		g_editorTempPath;
TCHAR		g_moduleName[MAX_PATH];
unsigned	g_findReplaceMsg;
HINSTANCE	g_hInstance;

const TCHAR* strVersion			=	_T("1.40");

const TCHAR* strAppName			=	_T("fSekrit");

const TCHAR* strSavePrompt		=	_T("Do you want to save your modifications?");

const TCHAR* strInvalidPassphrase=	_T("Invalid passphrase!");

const TCHAR* strErrorLoad		=	_T("Error loading file!");

const TCHAR* strErrorSave		=	_T("Error saving file!");

const TCHAR* strNotImplemented	=	_T("Not implemented yet! :(");

const TCHAR*	strStayTuned	=	_T("Stay Tuned...");

const TCHAR* strMalformedArg	=	_T("Malformed commandline argument!");

const TCHAR* strErrorErase		=	_T("Error erasing tempfile!");

const TCHAR* strErrorCopy		=	_T("Error copying file!");

const TCHAR* strErrorLaunch		= _T("Error launching file");

const TCHAR* strNewDocument		= _T("<new document>");

const TCHAR*	strFileAlreadyExists= _T("File already exists - overwrite?");

const TCHAR* strRichEditDLL		= _T("RichEd20.DLL");

const TCHAR* strErrorLoadRichEd	= _T("Error loading RichEdit control!");

const TCHAR* strErrorReadOnly	= _T("File is read-only.");

const TCHAR* strNoMoreMatches	= _T("No more matches.");

const TCHAR* strErrorBackwardsCompatible = _T("This version isn't backwards compatible with v1 files.");

const TCHAR* strRegConfigPath	= _T("Software\\flork.dk\\fSekrit");

const TCHAR* strNoSaveEmpty		= _T("I utterly refuse to save a blank Document :)");

unsigned g_ugly_w, g_ugly_h, g_ugly_x, g_ugly_y;
bool		g_ugly_maximized;

