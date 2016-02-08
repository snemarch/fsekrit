#pragma once
#ifndef idB5E7D11C_B830_4906_9C8BBF296AA5D5F2
#define idB5E7D11C_B830_4906_9C8BBF296AA5D5F2

#include <string>
#include <vector>

#include "plaintext.h"

#define lengthof(x) (sizeof(x)/sizeof(x[0]))

namespace util
{

size_t roundup(size_t val, size_t amt);

bool get_open_filename(HWND hwnd, xstring &name, const xstring &filters);
bool get_save_filename(HWND hwnd, xstring &name, const xstring &filters);

bool file_exists(const xstring &name);
bool file_copy(const xstring &target, const xstring &source);
bool file_copy_range(const xstring &target, const xstring &source, size_t ofs, size_t len);

void error_box(const TCHAR *message);

bool launch(const xstring &program, const xstring &parms, bool wait);

xstring get_base_path(const xstring& path);
xstring get_base_name(const xstring& path);
xstring get_temp_name(const xstring& path, const TCHAR* prefix, const TCHAR* suffix);

std::string bin2hex(const std::vector<char> &data);
std::vector<char> hex2bin(const xstring &data);

void CenterWindow(HWND centerOnhWnd, HWND hWnd);

void fill_random(void *buffer, size_t length);

void WideToNarrow(Plaintext& out, const Plaintext& in);
void NarrowToWide(Plaintext& out, const Plaintext& in);

} // util namespace

#endif // header
