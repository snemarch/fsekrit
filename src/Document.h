#pragma once
#ifndef id68CA79C7_E61D_41C5_A3664689F6AE0FE9
#define id68CA79C7_E61D_41C5_A3664689F6AE0FE9

#include <string>
#include <vector>

class Plaintext;

namespace document
{

struct DocumentImpl;

class Document
{
public:
	Document();
	~Document();

	/**
	* returns true if the document has a nonzero AES key.
	* @return true if document has a non-zero key
	*/
	bool	IsKeySet() const;

	/**
	* returns true if the decryption key is valid (ie., can be used to decrypt the
	* loaded blob).
	* @return true if key is valid
	*/
	bool	IsKeyValid() const;

	/**
	* @return filename of the current document
	*/
	const xstring&	GetName() const;

	/**
	* mouser wants these...
	*/
	void		SetMaximized(bool max);
	void		SetWidth(unsigned w);
	void		SetHeight(unsigned h);
	void		SetReadOnly(bool readonly);
	bool		GetMaximized() const;
	unsigned	GetWidth() const;
	unsigned	GetHeight() const;
	bool		GetReadOnly() const;


	/**
	* Empties content of current document and clears filename. Doesn't change encryption key.
	*/
	void	Clear();

	/**
	* clears the AES key
	*/
	void	ClearKey();

	/**
	* sets internal 256-bit AES key to SHA-256 hash of supplied ascii passphrase.
	* Does not modify current encrypted data. This is mostly useful for setting
	* initial key when IsKeyValid() returns false.
	* @param key key/passphrase to use
	*/
	void	SetKey(const Plaintext& key);

	/**
	* sets internal 256-bit AES key directly
	* @param key a vector containing a 256-bit AES key
	*/
	void	SetKeyRaw(const std::vector<char> &key);

	/**
	* re-encrypts document with new key. IsKeyValid() must be true before using this.
	* @param key key/passphrase to use
	*/
	void	ChangeKey(const Plaintext& key);

	/**
	* gets current Document text
	* @param text will receive the text
	*/
	void	GetText(Plaintext& text) const;

	/**
	* sets the Document text
	* @param text source text
	*/
	void	SetText(const Plaintext& text);

	/**
	* loads encrypted Document, either directly (.sekrit) or end-of-file data (.exe)
	* @param name name of file to load from
	* @return true on success, false on failure
	*/
	bool	Load(const xstring& name);

	/**
	* saves document. Involves spawning external process and terminating...
	* @return false on error
	*/
	bool	Save() const;

	/**
	* saves document with a new filename.
	* @param name filename
	* @return true on success, false on error
	*/
	bool	SaveAs(const xstring& name) const;

private:
	Document(const Document&);
	Document operator= (const Document&);

	bool	InternalSave(const xstring& name) const;

	DocumentImpl	*m_impl;
};

} // document namespace

#endif // header
