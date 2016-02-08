#pragma once
#ifndef id928C0462_D789_4CE0_A4047E042E07A019
#define id928C0462_D789_4CE0_A4047E042E07A019

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace font
{

enum FontStyle { FS_BOLD = 1, FS_ITALIC = 2, FS_UNDERLINE = 4, FS_STRIKEOUT = 8};

HFONT font_create(const TCHAR* a_type, const unsigned a_size, const unsigned a_style, const HWND a_hwnd=0);
bool do_choose_font(HWND hwnd, xstring& face, unsigned& size);
void font_load_settings(xstring& face, unsigned& size);
void font_save_settings(const xstring& face, const unsigned size);

}

#endif // header
