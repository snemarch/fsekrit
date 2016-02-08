#include "precompiled.h"

#include "file.h"

namespace file
{


File::File(TCHAR const* name, bool write, bool create)
{
	m_handle = ::CreateFile(name, write ? GENERIC_WRITE|GENERIC_READ : GENERIC_READ,
		write ? 0 : FILE_SHARE_READ, 0, create ? OPEN_ALWAYS : OPEN_EXISTING, 0, 0);
	if(!m_handle)
		throw FileError();
}


File::~File()
{
	if(m_handle)
		::CloseHandle(m_handle);
}


bool File::Read(void *buf, size_t size)
{
	assert(size <= std::numeric_limits<DWORD>::max());	
	DWORD outsize;

	if(!::ReadFile(m_handle, buf, static_cast<DWORD>(size), &outsize, 0))
		return false;
	if(size != outsize)
		return false;

	return true;
}


bool File::Write(void const *buf, size_t size)
{
	assert(size <= std::numeric_limits<DWORD>::max());	
	DWORD outsize;

	if(!::WriteFile(m_handle, buf, static_cast<DWORD>(size), &outsize, 0))
		return false;

	if(outsize != size)
		return false;

	return true;
}


bool File::Seek(size_t pos)
{
	if( INVALID_SET_FILE_POINTER ==
		::SetFilePointer(m_handle, static_cast<LONG>( pos ), 0, FILE_BEGIN) )
		return ( NO_ERROR == ::GetLastError() );
	else
		return true;
}


bool File::Truncate()
{
	return (0 != ::SetEndOfFile(m_handle));
}


size_t File::Tell()
{
	size_t offset;

	offset = ::SetFilePointer (m_handle, 0, 0, FILE_CURRENT);
	if (offset == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR)
		offset = 0;	//FIXME: need a way of reporting error - exceptions?

	return offset;
}


size_t File::GetSize()
{
	size_t offset;

	offset = ::GetFileSize(m_handle, 0);
	if (offset == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR)
		offset = 0; //FIXME: need a way of reporting error - exceptions?

	return offset;
}


} // file namespace
