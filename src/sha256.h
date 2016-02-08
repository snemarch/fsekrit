#pragma once
#ifndef idEB0EBA47_EA3A_4511_B9F7BDED77397714
#define idEB0EBA47_EA3A_4511_B9F7BDED77397714

namespace sha256
{

unsigned const hashbits = 256;

void hash(void *hash, const void *data, size_t size);

struct sha256_context;

class SHA256
{
public:
			SHA256();
			~SHA256();

	void	start();
	void	update(const void *input, size_t length);
	void	finish(void *hash);

private:
	sha256_context	*m_context;
};


} // sha256 namespace

#endif // header
