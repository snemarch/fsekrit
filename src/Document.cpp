#include "precompiled.h"
#include "Document.h"

#include "sekrit.h"
#include "File.h"
#include "FileEndData.h"
#include "globals.h"
#include "util.h"
#include "Plaintext.h"

namespace document
{

struct DocumentImpl
{
	sekrit::Sekrit	container;		// encrypted container
	xstring			fileName;		// full path of document
};


Document::Document() : m_impl(new DocumentImpl)
{
}


Document::~Document()
{
	delete m_impl;
}


bool Document::IsKeySet() const
{
	return m_impl->container.IsKeySet();
}


bool Document::IsKeyValid() const
{
	return m_impl->container.IsKeyValid();
}


const xstring& Document::GetName() const
{
	return m_impl->fileName;
}


void Document::SetMaximized(bool max)
{
	m_impl->container.SetMaximized(max);
}


void Document::SetWidth(unsigned w)
{
	m_impl->container.SetWidth(w);
}


void Document::SetHeight(unsigned h)
{
	m_impl->container.SetHeight(h);
}


void Document::SetReadOnly(bool readonly)
{
	m_impl->container.SetReadOnly(readonly);
}

bool Document::GetMaximized() const
{
	return m_impl->container.GetMaximized();
}


unsigned Document::GetWidth() const
{
	return m_impl->container.GetWidth();
}


unsigned Document::GetHeight() const
{
	return m_impl->container.GetHeight();
}


/**
* returns true if the document is read-only. For read-only documents, Save() won't
* work but SaveAs() will.
* @return true if document is read-only
*/
bool Document::GetReadOnly() const
{
	return m_impl->container.GetReadOnly();
}


void Document::Clear()
{
	m_impl->fileName.clear();
	m_impl->container.SetPlaintext(0);
}


void Document::ClearKey()
{
	m_impl->container.ClearKey();
}


void Document::SetKey(const Plaintext& key)
{
	m_impl->container.SetKey(key);
}




void Document::ChangeKey(const Plaintext& key)
{
	if( m_impl->container.IsEmpty() || !IsKeyValid() )
	{
		m_impl->container.SetKey(key);
		return;
	}

	Plaintext plaintext;

	m_impl->container.GetPlaintext(plaintext);
	m_impl->container.SetKey(key);
	m_impl->container.SetPlaintext(plaintext);
}


void Document::GetText(Plaintext& text) const
{
	m_impl->container.GetPlaintext(text);
}


void Document::SetText(const Plaintext& text)
{
	m_impl->container.SetPlaintext(text);
}


bool Document::Load(const xstring& name)
{
	bool gotData = false;
	std::vector<unsigned char> blob;

	if( sekrit::IsSekritContainer(name.c_str()) )
	{
		// ok, file has a SEKRiT header, read it directly.
		file::File f(name.c_str());

		blob.resize(f.GetSize());
		f.Read(&blob[0], f.GetSize());

		gotData = true;
	} else
	{
		// might be a file with appended data, so try loading that
		FileEndData::FileEndData fed(name, true);

		if( fed.HasData() )
			gotData = fed.ReadData(blob);
	}

	if(gotData)
	{
		m_impl->container.SetEncryptedBlob(blob);
		m_impl->fileName = name;
	}

	return gotData;
}


bool Document::Save() const
{
	if(GetReadOnly())
	{
		util::error_box(strErrorReadOnly);
		return false;
	}

	return InternalSave(m_impl->fileName);
}


bool Document::SaveAs(const xstring& name) const
{
	if(!InternalSave(name))
		return false;

	m_impl->fileName = name;
	return true;
}

/**
* performs an atomic save of the document - will not fubar the current document
* in case of an error underway. Achieves this by saving to a temp file, then
* deleting target file (if it exist) and renaming the temp file to target.
*/
bool Document::InternalSave(const xstring& name) const
{
	//TODO: implement NTFS transaction version of delete-and-rename sequence
	xstring tempFilePath = util::get_temp_name(util::get_base_path(name), _T("fSekrit"), _T("tmp"));
	if(!util::file_copy(tempFilePath, g_moduleName))
		return false;

	m_impl->container.SetWidth(g_ugly_w);
	m_impl->container.SetHeight(g_ugly_h);
	m_impl->container.SetMaximized(g_ugly_maximized);

	Plaintext text;
	GetText(text);
	m_impl->container.SetPlaintext(text);

	std::vector<unsigned char> blob;
	m_impl->container.GetEncryptedBlob(blob);

	FileEndData::FileEndData fed(tempFilePath, false);
	if(!fed.WriteData(blob))
	{
		util::error_box(strErrorSave);
		return false;
	}

	// data written to temp file, do renaming
	if(util::file_exists(name))
		::DeleteFile(name.c_str());

	if(! ::MoveFile(tempFilePath.c_str(), name.c_str()))
		return false;

	m_impl->fileName = name;
	return true;
}


} // document namespace
