#pragma once
#ifndef id166AA48C_D746_42BF_BCA2D8781C01EE30
#define id166AA48C_D746_42BF_BCA2D8781C01EE30

#include <string>
#include <vector>
#include <exception>


namespace FileEndData
{

struct FileEndDataImpl;

class FileEndData
{
public:
	~FileEndData();
	FileEndData(const xstring& fileName, bool readOnly);

	bool	HasData() const;
	size_t	GetExeSize() const;
	size_t	GetDataSize() const;
	size_t	GetDataOffset() const;
	bool	ReadData(std::vector<unsigned char> &data) const;
	bool	WriteData(std::vector<unsigned char> const &data);

private:
	FileEndDataImpl* m_impl;

	FileEndData(const FileEndData&);
	FileEndData& operator= (const FileEndData&);

	void	Initialize();
	void	ReadHeader(const xstring& fileName);
};


class FEDError:public std::exception
{
public:
	FEDError() : m_what("") {}
	FEDError(const char* what) : m_what(what) { }
	const char* what() const { return m_what; }

private:
	const char* m_what;
};


} // namespace

#endif // header
