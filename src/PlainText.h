#pragma once
#ifndef idB84EB647_5174_496F_A8E903C974E16A21
#define idB84EB647_5174_496F_A8E903C974E16A21

/*
PlainText: class for storing sensitive data. Contents will be cleared as needed
(on class destruction, resize).
*/

class PlaintextImpl;
class Plaintext
{
public:
	Plaintext();
	Plaintext(size_t size);

	~Plaintext();

	Plaintext(const Plaintext& other);
	Plaintext& operator= (Plaintext other);		// note: pass by value!

	/**
	* returns true if the buffer is empty
	*/
	bool	empty() const;

	/*
	* swap buffer contents
	*/
	void	swap(Plaintext &other);


	/*
	* clear buffer, set size=0
	*/
	void	clear();


	/**
	* resizes the buffer that holds the plaintext. Can only increase size.
	* copies old data, if any, to resized buffer.
	* @param amount new size for buffer, in bytes
	*/
	void	resize(size_t amount);


	/**
	* sets max buffer size and increases buffer. Can only increase
	* size.
	* @param amount new size for buffer, in bytes
	*/
	void	SetLimit(size_t amount);

	/**
	* get max buffer size
	*/
	size_t	GetLimit() const;
	
	/**
	* returns current buffer size, in bytes
	*/
	size_t	size() const;

	
	/**
	* append a block of data to the plaintext buffer
	* @param data pointer to data
	* @len length of buffer
	*/
	void	Append(void *data, size_t len);
	
	/**
	* non-const accessor
	*/
	void *data() const;

	// member functions for implementing free-standing operators
	bool	equals(const Plaintext& other) const;

private:

	int num;
	PlaintextImpl	*m_imp;
};

bool operator== (const Plaintext& lhs, const Plaintext& rhs);
bool operator!= (const Plaintext& lhs, const Plaintext& rhs);

namespace std
{

template<> inline void swap<Plaintext>(Plaintext& a, Plaintext& b)
{
	OutputDebugString(_T("specialized std::swap<Plaintext,Plaintext>"));
	a.swap(b);
}

}

inline void swap(Plaintext& a, Plaintext& b)
{
	OutputDebugString(_T("global swap<Plaintext,Plaintext>"));
	a.swap(b);
}



#endif // header
