#include "precompiled.h"

#include <stdlib.h>
#include <algorithm>

#include "plaintext.h"


static int num_pt = 0;
static int num_pti= 0;


static void dbgmsg(TCHAR *s)
{
#ifdef _DEBUG
	OutputDebugString(s);
//	OutputDebugString("\n");
//	MessageBox(0, s, "like, error", MB_OK);
#else
	UNUSED(s);
#endif
}


class PlaintextImpl
{
public:
	PlaintextImpl() : plaintext(0), size(0), limit(0), num(num_pti++)
	{
		TCHAR buf[500];
		wsprintf(buf, _T("creating PlaintextImpl %d"), num);
		dbgmsg(buf);
		
	}

	PlaintextImpl(const PlaintextImpl& other) : plaintext(0), size(0), limit(0), num(num_pti++)
	{
		TCHAR buf[500];
		wsprintf(buf, _T("copy PlaintextImpl %d from %d(%p,%p,%d)"), num, other.num, &other, other.plaintext, other.limit);
		dbgmsg(buf);

		plaintext = new char[other.limit];
		limit = other.limit;
		size = other.size;
		memcpy(plaintext, other.plaintext, size);
	}


	~PlaintextImpl()
	{
		TCHAR buf[500];
		wsprintf(buf, _T("destroying PlaintextImpl %d (%p,%d)"), num, plaintext, limit);
		dbgmsg(buf);

		if(plaintext && limit)
			memset(plaintext, 0, limit);

		delete[] plaintext;
	}


	void swap(PlaintextImpl& other)
	{
		using std::swap;

		OutputDebugString(_T("PlaintextImpl::swap"));
		swap(plaintext, other.plaintext);
		swap(size, other.size);
		swap(limit, other.limit);
	}


	PlaintextImpl& operator=(const PlaintextImpl& other)
	{
		TCHAR buf[500];
		wsprintf(buf, _T("PlaintextImpl%d = PlaintextImpl%d"), num, other.num);
		dbgmsg(buf);

		using std::swap;

		if(this == &other)
			return *this;

		void *tmp = new char[other.limit];
		swap(plaintext, tmp);
		limit = other.limit;
		size = other.size;
		delete[] tmp;
		memcpy(plaintext, other.plaintext, size);
		return *this;
	}

	void clear()
	{
		if(plaintext && size)
			memset(plaintext, 0, size);
		size = 0;
	}

	void	*plaintext;
	size_t	size;
	size_t	limit;

	int		num;
};


namespace std {
template<> inline void swap<PlaintextImpl>(PlaintextImpl& a, PlaintextImpl& b)
{
	OutputDebugString(_T("specialized std::swap<PlaintextImpl,PlaintextImpl>"));
	a.swap(b);
}

}

inline void swap(PlaintextImpl& a, PlaintextImpl& b)
{
	OutputDebugString(_T("global swap<PlaintextImpl,PlaintextImpl>"));
	a.swap(b);
}



Plaintext::Plaintext() : m_imp(0), num(num_pt++)
{
	TCHAR buf[500];
	wsprintf(buf, _T("creating Plaintext %d"), num);
	dbgmsg(buf);

	m_imp = new PlaintextImpl;
}


Plaintext::Plaintext(size_t size) : m_imp(0), num(num_pt++)
{
	TCHAR buf[500];
	wsprintf(buf, _T("creating Plaintext %d (size ctor)"), num);
	dbgmsg(buf);

	m_imp = new PlaintextImpl;
	SetLimit(size);
}



Plaintext::Plaintext(const Plaintext& other) : m_imp(0), num(num_pt++)
{
	TCHAR buf[500];
	wsprintf(buf, _T("creating Plaintext %d from %d"), num, other.num);
	dbgmsg(buf);

	m_imp = new PlaintextImpl(*other.m_imp);
}


Plaintext::~Plaintext()
{
	TCHAR buf[500];
	wsprintf(buf, _T("destroying Plaintext %d, imp=%p"), num, m_imp);
	dbgmsg(buf);

	clear();
	delete m_imp;
}


Plaintext& Plaintext::operator= (Plaintext other)
{
	swap(other);
	return *this;
}







size_t Plaintext::GetLimit() const
{
	return m_imp->limit;
}


size_t	Plaintext::size() const
{
	return m_imp->size;
}


bool Plaintext::empty() const
{
	return !m_imp->size;
}


void Plaintext::swap(Plaintext &other)
{
	using std::swap;

	OutputDebugString(_T("Plaintext::swap"));
	swap(m_imp, other.m_imp);
}


void Plaintext::clear()
{
	m_imp->clear();
}


void Plaintext::resize(size_t amount)
{
	if (amount == m_imp->size)
		return;

	if(amount < m_imp->size)
	{
		TCHAR buf[500];
		wsprintf(buf, _T("resize from %d to %d, clear %d bytes"), m_imp->size, amount, m_imp->limit-amount);
		dbgmsg(buf);

		// shrink size. Buffer never shrinks, just clear out excess data.
		memset(static_cast<char*>(m_imp->plaintext) + amount, 0, m_imp->limit-amount);
	} else {
		// (potentially) grow buffer
		SetLimit(amount);
	}

	m_imp->size = amount;
}


void Plaintext::SetLimit(size_t amount)
{
	if(amount <= m_imp->limit)
		return;

	void *tmp = new char[amount];
	if(!tmp)
		return;	// should throw exception

	// not necessary to clear ]m_size;m_limit] range
	if(m_imp->size)
		memcpy(tmp, m_imp->plaintext, m_imp->size);

	// swap in new buffer, clear out old buffer
	using std::swap;

	swap(m_imp->plaintext, tmp);
	memset(tmp, 0, m_imp->limit);

	m_imp->limit = amount;

	delete[] tmp;
}


void Plaintext::Append(void *data, size_t len)
{
	size_t oldsize = m_imp->size;

	resize(m_imp->size + len);
	memcpy(static_cast<char*>(m_imp->plaintext) + oldsize, data, len);
}


void *Plaintext::data() const
{
	return m_imp->plaintext;
}


bool Plaintext::equals(const Plaintext& other) const
{
	if(m_imp->size != other.m_imp->size)
		return false;

	return (0 == memcmp(m_imp->plaintext, other.m_imp->plaintext, m_imp->size));
}


bool operator== (const Plaintext& lhs, const Plaintext& rhs)
{
	return lhs.equals(rhs);
}

bool operator!= (const Plaintext& lhs, const Plaintext& rhs)
{
	return !(lhs == rhs);
}
