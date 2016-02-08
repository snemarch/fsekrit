#include "precompiled.h"

#include "hyperlink.h"
#include "DlgAbout.h"
#include "resource.h"
#include "util.h"


namespace DlgAbout
{

static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void invoke(HWND hwndParent)
{
	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(DLG_ABOUT),
			hwndParent, DialogProc, 0);
}


INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static CHyperLink	h1, h2;

	switch(msg)
	{
	case WM_INITDIALOG:
		TCHAR temp[128];
		GetDlgItemText(hwnd, IDC_LINK1, temp, lengthof(temp));
		h1.ConvertStaticToHyperlink(hwnd, IDC_LINK1, temp);
		GetDlgItemText(hwnd, IDC_LINK2, temp, lengthof(temp));
		h2.ConvertStaticToHyperlink(hwnd, IDC_LINK2, temp);
		return TRUE;

	case WM_CLOSE:
		EndDialog(hwnd, FALSE);
		return TRUE;

	case WM_COMMAND:
		if(lp)
		{
			switch(LOWORD(wp))
			{
			case IDOK:
				EndDialog(hwnd, 0);
				return TRUE;
			}
		}
	}

	return FALSE;
}

} // DlgAbout namespace
