#include "precompiled.h"

#include "FileEndData.h"
#include "File.h"


namespace FileEndData
{

struct FileEndDataImpl
{
	xstring		fileName;
	bool		hasData;
	bool		isReadOnly;
	size_t		dataBegin;		// data begin = original exe size
	size_t		dataSize;
};

static const unsigned MAGIC = 0xDEADBEEF;

#pragma pack(push, 1)
struct EndDataHeader
{
	unsigned	magic;
	unsigned	size;
};
#pragma pack(pop)



bool FileEndData::HasData() const
{
	return m_impl->hasData;
}


size_t FileEndData::GetExeSize() const
{
	return m_impl->dataBegin;
}


size_t FileEndData::GetDataSize() const
{
	return m_impl->dataSize;
}


size_t FileEndData::GetDataOffset() const
{
	return m_impl->dataBegin;
}


FileEndData::FileEndData(const xstring& fileName, bool readOnly) : m_impl(new FileEndDataImpl)
{
	m_impl->fileName = fileName;
	m_impl->hasData = false;
	m_impl->isReadOnly = readOnly;
	m_impl->dataBegin = 0;
	m_impl->dataSize = 0;

	ReadHeader(fileName);
}


bool FileEndData::ReadData(std::vector<unsigned char> &data) const
{
	//TODO: should we bother making this exception safe?
	if(!m_impl->hasData)
		return false;

	file::File f(m_impl->fileName.c_str());
	if(!f.Seek(m_impl->dataBegin))
		return false;

	data.resize(m_impl->dataSize);
	return f.Read(&data[0], m_impl->dataSize);
}


bool FileEndData::WriteData(std::vector<unsigned char> const &data)
{
	assert(data.size() <= std::numeric_limits<unsigned>::max());
	//TODO: should we bother making this exception safe?
	if(m_impl->isReadOnly)
		return false;

	EndDataHeader hdr;
	hdr.magic = MAGIC;
	hdr.size = static_cast<unsigned>(data.size());

	file::File f(m_impl->fileName.c_str(), true, true);

	if(!m_impl->hasData)
		m_impl->dataBegin = f.GetSize();

	f.Seek(m_impl->dataBegin);
	if( !f.Write(&data[0], data.size()) )
		return false;

	if( !f.Write(&hdr, sizeof(hdr)) )
		return false;

	m_impl->dataSize = data.size();
	m_impl->hasData = true;

	return f.Truncate();
}


FileEndData::~FileEndData()
{
	delete m_impl;
}


void FileEndData::ReadHeader(const xstring& fileName)
{
	file::File fin( fileName.c_str() );

	// file too small?
	if(!fin.Seek( fin.GetSize() - sizeof(EndDataHeader) ))
		return;

	EndDataHeader hdr;
	if(!fin.Read(&hdr, sizeof(hdr) ) )
		throw FEDError("FileEndData::readheader");

	// if magic isn't right, there's currently no end-file data
	if(hdr.magic != MAGIC)
		return;

	// invalid header?
	if( hdr.size > (fin.GetSize() - sizeof(hdr)) )
		throw FEDError("FileEndData::header");

	// got a valid header, fill m_impl
	m_impl->hasData = true;
	m_impl->dataBegin = fin.Tell() - hdr.size - sizeof(hdr);
	m_impl->dataSize = hdr.size;
}


} // namespace
