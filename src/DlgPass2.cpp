#include "precompiled.h"

#include "DlgPass2.h"
#include "resource.h"
#include "util.h"


namespace DlgPass2
{

static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

struct InitStruct
{
	Plaintext *passphrase;
};

bool invoke(Plaintext &passphrase)
{
	InitStruct	initstruct;
	INT_PTR		result;

	initstruct.passphrase = &passphrase;
	result = DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(DLG_PASSPHRASE2),
			0, DialogProc, reinterpret_cast<LPARAM>( &initstruct ));

	return (result != 0);
}


void GetTextString(HWND hwnd, Plaintext &text)
{
	unsigned len = static_cast<unsigned>( SendMessage(hwnd, WM_GETTEXTLENGTH, 0, 0) + 1 );

	text.resize(len * sizeof(TCHAR));
	GetWindowText(hwnd, reinterpret_cast<PTCHAR>( text.data() ), len);
}


INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static Plaintext *passphrase;

	switch(msg)
	{
	case WM_INITDIALOG:
		passphrase = reinterpret_cast<InitStruct*>(lp)->passphrase;
		return TRUE;

	case WM_CLOSE:
		EndDialog(hwnd, FALSE);
		return TRUE;

	case WM_COMMAND:
		if(lp)
		{
			switch(LOWORD(wp))
			{
				case BTN_OK:
				{
					Plaintext	pass1, pass2;

					GetTextString(GetDlgItem(hwnd, IDC_EDIT1), pass1);
					GetTextString(GetDlgItem(hwnd, IDC_EDIT2), pass2);

					if( (pass1.size() != pass2.size()) || (pass1 != pass2) )
					{
						util::error_box(_T("Passphrases don't match"));
						return TRUE;
					}

					passphrase->swap(pass1);
					EndDialog(hwnd, TRUE);
					return TRUE;
				}

			case BTN_CANCEL:
				EndDialog(hwnd, FALSE);
				return TRUE;
			}
		}
	}

	return FALSE;
}

} // DlgPass2 namespace
