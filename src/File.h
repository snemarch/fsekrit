#pragma once
#ifndef idB7EB05BB_A4C8_40F8_A7CFC58E8DEB2880
#define idB7EB05BB_A4C8_40F8_A7CFC58E8DEB2880

#include <exception>

namespace file
{

class FileError:public std::exception
{
public:
	FileError() : m_what("") {}
	FileError(const char* what) : m_what(what) { }
	const char* what() const { return m_what; }

private:
	const char* m_what;
};



class File
{
public:
	File(TCHAR const* name, bool write=false, bool create=false);
	~File();

	bool	Read(void *buf, size_t size);
	bool	Write(void const *buf, size_t size);
	bool	Seek(size_t pos);
	bool	Truncate();
	size_t	Tell();
	size_t	GetSize();

private:
	File(const File&);
	File operator=(const File&);

	void	*m_handle;

};

};

#endif // header
