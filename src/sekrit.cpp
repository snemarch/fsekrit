#include "precompiled.h"
#include "sekrit.h"

#include "util.h"
#include "aes/aescpp.h"
#include "File.h"
#include "sha256.h"
#include "Plaintext.h"

namespace sekrit
{

const char *const header_v1_signature = "SEKRiT*1";
const char *const header_v2_signature = "SEKRiT*2";

const unsigned FL_READONLY = 1;
const unsigned FL_MAXIMIZED = 2;				// [1.2] - is window maximized?

#pragma pack(push, 1)
struct FileHeaderV1
{
	char signature[8];							// header_v1_signature
	char hash[sha256::hashbits/8];				// plaintext hash
	unsigned size;								// size of data, padded to crypt blocksize
	unsigned realsize;							// "real" size of data, unpadded
	unsigned flags;								// bitmask of FL_* constants 
	unsigned w, h;								// saved width and height... mouser wants this
};

struct FileHeaderV2
{
	char signature[8];							// header_v2_signature
	char hash[sha256::hashbits/8];				// plaintext hash
	unsigned char iv[aes_blocksize];			// initial encryption vector (added in v2)
	unsigned size;								// size of data, padded to crypt blocksize
	unsigned realsize;							// "real" size of data, unpadded
	unsigned flags;								// bitmask of FL_* constants 
	unsigned w, h;								// saved width and height... mouser wants this
};
#pragma pack(pop)


struct SekritImpl
{
	std::vector<unsigned char>	data;			// encrypted data blob
	unsigned char	key[aes_keybits/8];			// currently used encryption key
	bool			keyHasBeenSet;
	FileHeaderV2	header;
};


Sekrit::Sekrit(const Plaintext& key) : m_impl(new SekritImpl)
{
	//memset(m_impl, 0, sizeof(*m_impl));	// I'm a FUCKTARD!
	Initialize();
	SetKey(key);
}


Sekrit::Sekrit() : m_impl(new SekritImpl)
{
	//memset(m_impl, 0, sizeof(*m_impl));	// I'm a FUCKTARD!
	Initialize();
}


Sekrit::~Sekrit()
{
	delete m_impl;
}

void Sekrit::ClearKey()
{
	memset(m_impl->key, 0, sizeof(m_impl->key));
	m_impl->keyHasBeenSet = false;
}


void Sekrit::SetKey(const Plaintext& key)
{
	sha256::hash(m_impl->key, key.data(), key.size());
	m_impl->keyHasBeenSet = true;
}


bool Sekrit::IsEmpty() const
{
	return m_impl->data.empty();
}


bool Sekrit::IsKeySet() const
{
	return m_impl->keyHasBeenSet;
}


bool Sekrit::IsKeyValid() const
{
	if(!m_impl->keyHasBeenSet)
		return false;

	Plaintext plaintext = DoDecrypt();
	bool IsValid = VerifyHash(plaintext, m_impl->header.iv);
	return IsValid;
}


void Sekrit::SetMaximized(bool max)
{
	if(max)
		m_impl->header.flags |= FL_MAXIMIZED;
	else
		m_impl->header.flags &= ~ FL_MAXIMIZED;
}


void Sekrit::SetWidth(unsigned w)
{
	m_impl->header.w = w;
}


void Sekrit::SetHeight(unsigned h)
{
	m_impl->header.h = h;
}


void Sekrit::SetReadOnly(bool readonly)
{
	if(readonly)
		m_impl->header.flags |= FL_READONLY;
	else
		m_impl->header.flags &= ~FL_READONLY;
}


bool Sekrit::GetMaximized() const
{
	return	(m_impl->header.flags & FL_MAXIMIZED) == FL_MAXIMIZED;
}


unsigned Sekrit::GetWidth() const
{
	return 	m_impl->header.w;
}


unsigned Sekrit::GetHeight() const
{
	return 	m_impl->header.h;
}


bool Sekrit::GetReadOnly() const
{
	return (m_impl->header.flags & FL_READONLY) == FL_READONLY;
}


bool Sekrit::GetPlaintext(Plaintext& data) const
{
	if( !m_impl->data.empty() )
	{
		char hash[sha256::hashbits/8];
		Plaintext plaintext(DoDecrypt());
		ComputeHash(hash, plaintext, m_impl->header.iv);

		if( 0 == memcmp(hash, m_impl->header.hash, sizeof(hash)) )
		{
			data.swap(plaintext);
			return true;
		} else
			return false;
	} else
	{
		data.clear();
		return true;
	}
}


void Sekrit::SetPlaintext(const Plaintext& plaintext)
{
	if(plaintext.empty())
	{
		// input empty, reset content
		m_impl->header.realsize = m_impl->header.size = 0;
		memset( m_impl->header.hash, 0, sizeof(m_impl->header.hash) );
		m_impl->data.clear();
	} else
	{
		DoEncrypt(plaintext);
		ComputeHash(m_impl->header.hash, plaintext, m_impl->header.iv);
	}
}



bool Sekrit::GetEncryptedBlob(std::vector<unsigned char> &data) const
{
	data.resize(sizeof(FileHeaderV2) + m_impl->data.size());

	memcpy(&data[0], &m_impl->header, sizeof(m_impl->header));
	memcpy(&data[sizeof(m_impl->header)], &m_impl->data[0], m_impl->data.size());

	return true;
}


bool Sekrit::SetEncryptedBlob(const std::vector<unsigned char> &data)
{
	FileHeaderV2 hdr;

	if(data.size() < sizeof(hdr))
		return false;

	memcpy(&hdr, &data[0], sizeof(hdr));
	if(0 != memcmp(hdr.signature, header_v2_signature, sizeof(hdr.signature)) )
		return false;

	if(data.size() != sizeof(hdr) + hdr.size)
		return false;

	memcpy(&m_impl->header, &hdr, sizeof(hdr) );
	m_impl->data.resize(hdr.size);
	memcpy(&m_impl->data[0], &data[sizeof(hdr)], hdr.size);

	return true;
}


void Sekrit::Initialize()
{
	memset(m_impl->key, 0, sizeof(m_impl->key));
	m_impl->keyHasBeenSet = false;

	memset(&m_impl->header, 0, sizeof(m_impl->header));
	memcpy(m_impl->header.signature, header_v2_signature, sizeof(m_impl->header.signature) );
}


void Sekrit::ComputeHash(char *outputhash, const Plaintext& plaintext, const unsigned char *iv) const
{
	sha256::SHA256	hash;

	hash.start();
	hash.update(plaintext.data(), plaintext.size() );
	hash.update(iv, aes_blocksize);
	hash.finish(outputhash);
}


bool Sekrit::VerifyHash(const Plaintext& plaintext, const unsigned char *iv) const
{
	char hash[sha256::hashbits/8];

	ComputeHash(hash, plaintext, iv);
	return( 0 == memcmp(hash, m_impl->header.hash, sizeof(hash)) );
}


void Sekrit::DoEncrypt(const Plaintext& plaintext)
{
	const size_t roundedsize = util::roundup(plaintext.size(), aes_blocksize);

	assert(plaintext.size() <= std::numeric_limits<unsigned>::max());
	assert(roundedsize <= std::numeric_limits<unsigned>::max());


	m_impl->header.realsize = static_cast<unsigned>( plaintext.size() );
	m_impl->header.size = static_cast<unsigned>( roundedsize );
	m_impl->data.resize( m_impl->header.size );

	// Fill CBC-mode InitializationVector with random bytes. Notice that we use
	// a "tempiv" and don't just work on header.iv, since cbc_encrypt works directly
	// on the buffer (which would leave us with a modified IV in the header - bad.)
	unsigned char tempiv[sizeof(m_impl->header.iv)];
	util::fill_random( m_impl->header.iv, sizeof(m_impl->header.iv) );
	memcpy(tempiv, m_impl->header.iv, sizeof(tempiv) );

	AESencrypt	crypt;
	crypt.key256(m_impl->key);
	crypt.cbc_encrypt(static_cast<unsigned char*>(plaintext.data()),
		&m_impl->data[0], m_impl->header.size, tempiv);
}


Plaintext Sekrit::DoDecrypt() const
{
	// Same notes apply here as in DoEncrypt()

	unsigned char tempiv[sizeof(m_impl->header.iv)];
	memcpy(tempiv, m_impl->header.iv, sizeof(tempiv) );

	Plaintext plaintext;
	plaintext.resize(m_impl->header.size);

	assert(plaintext.size() <= std::numeric_limits<size_t>::max() );

	AESdecrypt	crypt;
	crypt.key256(m_impl->key);
	crypt.cbc_decrypt(&m_impl->data[0],
		static_cast<unsigned char*>(plaintext.data()), static_cast<int>(plaintext.size()), tempiv);

	plaintext.resize(m_impl->header.realsize);
	return plaintext;
}


bool IsSekritContainer(TCHAR const* filename)
{
	file::File f(filename);

	FileHeaderV2 hdr;

	if( !f.Read(&hdr, sizeof(hdr)) )
		return false;

	return (0 == memcmp(hdr.signature, header_v2_signature, sizeof(hdr.signature)) );
}


} // sekrit namespace

