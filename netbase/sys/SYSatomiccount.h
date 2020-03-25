#ifndef SYSATOMICCOUNT_H_
#define SYSATOMICCOUNT_H_
#include <ext/atomicity.h>
using __gnu_cxx::__atomic_add;
using __gnu_cxx::__exchange_and_add;

class SYSatomiccount
{
public:
	SYSatomiccount() : value_(0) {}

	explicit SYSatomiccount( long v ) : value_( static_cast< int >( v ) ) {}

	long operator++()
	{
		return __exchange_and_add( &value_, +1 ) + 1;
	}

	long operator--()
	{
		return __exchange_and_add( &value_, -1 ) - 1;
	}

	long add(long n)
	{
		return __exchange_and_add( &value_, n ) + n;
	}

	operator long() const
	{
		return __exchange_and_add( &value_, 0 );
	}

private:

	SYSatomiccount(SYSatomiccount const &);
	SYSatomiccount & operator=(SYSatomiccount const &);

	mutable _Atomic_word value_;
};




#endif /* SYSatomiccount_H_ */
