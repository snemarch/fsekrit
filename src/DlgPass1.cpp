#include "precompiled.h"

#include "DlgPass1.h"
#include "resource.h"


namespace DlgPass1
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
	result = DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(DLG_PASSPHRASE1),
			0, DialogProc, reinterpret_cast<LPARAM>( &initstruct ));

	return (result != 0);
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
					unsigned len = static_cast<unsigned>( SendDlgItemMessage(hwnd, IDC_EDIT1, WM_GETTEXTLENGTH, 0, 0) + 1 );
					passphrase->resize(len);
					GetDlgItemText(hwnd, IDC_EDIT1, reinterpret_cast<PTCHAR>( passphrase->data() ), len);

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

} // DlgPass1 namespace
