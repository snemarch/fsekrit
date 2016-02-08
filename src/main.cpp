#include "precompiled.h"

#include "aes/aes.h"
#include "editor.h"
#include "DlgPass1.h"
#include "File.h"
#include "FindReplace.h"
#include "FileEndData.h"
#include "globals.h"
#include "resource.h"
#include "sekrit.h"
#include "util.h"



// RETRYCOUNT: times to retry write/erase operations. The parent process might not
// have exited yet before we try the operation, which will then fail because
// the EXE file is still locked. Thus we try-sleep-retry until the operation
// succeeds, or count==0.
const unsigned RETRYCOUNT = 30;


void handle_edit(const TCHAR *filename)
{
	g_documentFilePath.assign(filename);
	g_editorMode = true;
}


void handle_erase(const TCHAR *filename)
{
	unsigned count = RETRYCOUNT;

	// erase argument
	while(count--)
	{
		if( !util::file_exists(filename) )
			exit(-1);

		if(DeleteFile(filename))
			exit(0);

		Sleep(100);
	}

	// okay, operation wasn't successful...
	util::error_box(strErrorErase);
	exit(-1);
}


void handle_wipe(const TCHAR *filename)
{
	UNUSED(filename);
}


typedef void (*CommandProc)(const TCHAR*);

struct CommandArg
{
	TCHAR		*command;
	CommandProc	proc;
};

CommandArg commands[] =
{
	{ _T("-edit"),	handle_edit },
	{ _T("-erase"),	handle_erase }
//	{ "-wipe",	handle_wipe }
};


void HandleArg(const TCHAR *arg)
{
	for(unsigned i=0; i<lengthof(commands); i++)
	{
		if(StrStrI(arg, commands[i].command))
		{
			const TCHAR *s = _tcschr(arg, _T(':'));
			if(!s)
			{
				util::error_box(strMalformedArg);
				exit(-1);
			}
			commands[i].proc(s+1);
		}
	}
}


void CheckSpecialArgs()
{
	if(__argc > 1)
		for(int i=1; i < __argc; i++)
#ifdef _UNICODE
			HandleArg(__wargv[i]);
#else
			HandleArg(__argv[i]);
#endif
}


bool CheckAndLoadEmbeddedDocument(xstring fn, std::auto_ptr<document::Document> &doc)
{
	FileEndData::FileEndData fed( fn, true );

	if(fed.HasData())
	{
		if(!doc->Load(fn))
		{
			util::error_box(strErrorLoad);
			exit(-1);
		}

		// keep asking user for passphrase until he cancels or inputs correct passphrase.
		for(;;)
		{
			if(!doc->IsKeyValid())
			{
				Plaintext passphrase;

				if(!DlgPass1::invoke(passphrase))
					exit(0);

				doc->SetKey(passphrase);
				if(!doc->IsKeyValid())
				{
					util::error_box(strInvalidPassphrase);
					continue;
				}
			}
			return true;
		}
	} else
		return false;
}


void MessageLoop(HWND hwndMain)
{
	HACCEL accel = 
	LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	// DON'T filter by hwnd, otherwise messageloop will never terminate.
	MSG	msg;
	while(GetMessage(&msg, 0, 0, 0) != 0)
	{
		if( FindReplace::Instance()->MessageLoopHook(msg) )
			continue;

		if(!TranslateAccelerator(hwndMain, accel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}


void WindowsInitialization()
{
	g_hInstance = GetModuleHandle(0);
	GetModuleFileName(g_hInstance, g_moduleName, lengthof(g_moduleName));
	InitCommonControls();

	g_findReplaceMsg = ::RegisterWindowMessage(FINDMSGSTRING);

	if(!LoadLibrary(strRichEditDLL))
	{
		util::error_box(strErrorLoadRichEd);
		exit(-1);
	}

	srand( GetTickCount() );

	// initialize "where to store temporary editor executable" path. This used
	// to simply be %TEMP%, but for USB portable mode (and to shut up some silly
	// HIPS systems), we allow the use of a special file in the launched-from
	// folder to say "hey, run in portable mode and use this path instead."
	xstring localPath = util::get_base_path(g_moduleName);
	xstring localConf(localPath);
	localConf.append(_T("fSekrit.portable"));
	if(util::file_exists(localConf))
		g_editorTempPath = localPath;
	else
	{
		TCHAR buf[MAX_PATH];
		GetTempPath(sizeof(buf), buf);
		g_editorTempPath.assign(buf);
	}
}


bool CopyAndSpawn()
{
	FileEndData::FileEndData fed( xstring(g_moduleName), true );
	xstring		spawnedEditor;

	//TODO: make sure temp file doesn't already exist!
	spawnedEditor = util::get_temp_name(g_editorTempPath, _T("fSekrit"), _T("exe"));

	if(!util::file_copy_range(spawnedEditor.c_str(), g_moduleName, 0, fed.GetExeSize()) )
	{
		util::error_box(strErrorCopy);
		return false;
	}

	xstring parms(_T("-edit:\""));
	parms.append(g_moduleName);
	parms.append(_T("\""));

	util::launch(spawnedEditor, parms, false);

	return true;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WindowsInitialization();

	gen_tabs();								// initialize AES tables
	CheckSpecialArgs();						// handle commandline

	std::auto_ptr<document::Document>	doc(new document::Document);

	if(g_documentFilePath.empty())
	{
		// haven't got a filename, check for embedded data. If we've got embedded data,
		// make a temp copy of our exe, and launch that with "-edit". If no embedded data,
		// we'll end up with a blank note instead.

		FileEndData::FileEndData fed( xstring(g_moduleName), true );
		if(fed.HasData())
			return CopyAndSpawn() ? 0 : -1;
	} else
	{
		// We've got a filename from commandline, work with that file.
		if(!CheckAndLoadEmbeddedDocument(g_documentFilePath, doc))
		{
			util::error_box(strErrorLoad);
			return -1;
		}
	}

	// Now doc is either loaded from our own executable, or blank - start the editor.
	editor::Editor edit(doc);

	if(!g_documentFilePath.empty())
		edit.UpdateTitle(g_documentFilePath.c_str());

	MessageLoop(edit.GetHWND());

	if(g_editorMode)
	{
		// We were originally launched with "-edit" argument, which means we're running
		// from a temp file that should be erased. Launch original process to delete temp.
		//TODO: what if a nosy user is messing with commandline args?
		xstring args(_T("-erase:\""));
		args.append(g_moduleName);
		args.append(_T("\""));

		util::launch(g_documentFilePath, args, false);
	}

	return 0;
}
