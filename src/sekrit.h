#pragma once
#ifndef id441EC4BE_2ED1_4832_836A662966CA20B1
#define id441EC4BE_2ED1_4832_836A662966CA20B1

#include <vector>

class Plaintext;

namespace sekrit
{

struct SekritImpl;

class Sekrit
{
	// disallow copy construction and assignment
	Sekrit(const Sekrit&);
	Sekrit operator= (const Sekrit&);
public:
	Sekrit(const Plaintext& key);
	Sekrit();
	~Sekrit();

	/**
	* @return true if blob is empty
	*/
	bool	IsEmpty() const;

	/**
	* @return true if key has been set
	*/
	bool	IsKeySet() const;

	/**
	* Checks whether blob can be decrypted with current key.
	* @return true if key is valid
	*/
	bool		IsKeyValid() const;

	void		SetMaximized(bool max);
	void		SetWidth(unsigned w);
	void		SetHeight(unsigned h);
	void		SetReadOnly(bool readonly);
	bool		GetMaximized() const;
	unsigned	GetWidth() const;
	unsigned	GetHeight() const;
	bool		GetReadOnly() const;


	/**
	* decrypts plaintext and returns it
	* @param data output; receives plaintext
	* @return true on success, false on decrypt error
	*/
	bool	GetPlaintext(Plaintext& data) const;

	/**
	* encrypts plaintext and stores encrypted result internally
	* @param data plaintext to be encrypted
	*/
	void	SetPlaintext(const Plaintext& plaintext);


	bool	GetEncryptedBlob(std::vector<unsigned char> &data) const;
	bool	SetEncryptedBlob(const std::vector<unsigned char> &data);

	/**
	* clears the AES keys
	*/
	void	ClearKey();

	/**
	* sets internal 256-bit AES key to SHA-256 hash of supplied ascii passphrase.
	* Does not modify current encrypted data. This is mostly useful for setting
	* initial key.
	* @param key key/passphrase to use
	*/
	void	SetKey(const Plaintext& key);

private:
	void	ComputeHash(char *outputhash, const Plaintext& plaintext, const unsigned char *iv) const;
	bool	VerifyHash(const Plaintext& plaintext, const unsigned char *iv) const;

	void	DoEncrypt(const Plaintext& plaintext);
	Plaintext DoDecrypt() const;

	void	Initialize();
	SekritImpl *m_impl;
};

bool IsSekritContainer(TCHAR const* filename);

} // sekrit namespace

#endif // header
