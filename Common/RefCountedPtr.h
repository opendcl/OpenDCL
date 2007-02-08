#pragma once

// This template class defines a reference counted pointer that automatically deletes the object pointed 
// to when its reference count reaches zero. Creating a copy of the pointer class increments the reference 
// count, thus ensuring that the object stays alive as long as there are any outstanding pointers to it.
//
// by Owen Wengerd, 2007-01-29
//
// Usage:
// class CMyObject;
// RefCountedPtr< MyObject > pMyObject;

class RefCounterBase
{
private:
	ULONG ctRef;

public:
	RefCounterBase() : ctRef( 1 ) {}
	virtual ~RefCounterBase(void) {}

private:
	// prevent copy and assignment
	RefCounterBase(RefCounterBase & src);
	RefCounterBase& operator=(RefCounterBase & src);

	// reference counting
public:
	ULONG AddRef()
		{
			if( ctRef == ~0 )
				return ctRef;
			return ctRef++;
		}
	ULONG Release()
		{
			if( ctRef == ~0 )
				return ctRef;
			ULONG ct = --ctRef;
			if( 0 == ctRef )
				delete this;
			return ct;
		}
	void Lock()
		{
			ctRef = ~0;
		}
	RefCounterBase* Copy()
		{
			AddRef();
			return this;
		}
};

template< typename T >
class RefCounter : public RefCounterBase
{
private:
	T* pT;

public:
	RefCounter<T>( T* pTarget ) : pT( pTarget ) {}
	virtual ~RefCounter<T>(void) { delete pT; }

private:
	// prevent copy and assignment
	RefCounter<T>(RefCounter<T> & src);
	RefCounter<T>& operator=(RefCounter<T> & src);

public:
	bool isNull() const { return (pT? false : true); }

	operator const T* () const { return pT; }
	operator T* () { return pT; }

  const T* operator->() const { return pT; }
  T* operator->() { return pT; }

	bool operator ==( const RefCounter<T>& src ) const
		{
			return (*pT == *src.pT);
		}

	// reference counting
public:
	RefCounter<T>* Copy()
		{
			AddRef();
			return this;
		}
};

template< typename T, typename _R = RefCounter<T> >
class RefCountedPtr
{
private:
	_R* pW;

public:
	RefCountedPtr<T,_R>()
	: pW( NULL )
		{
		}
	RefCountedPtr<T,_R>( T* pTarget )
	: pW( pTarget? new _R( pTarget ) : NULL )
		{
		}
	virtual ~RefCountedPtr<T,_R>(void)
		{
			if( pW )
				pW->Release();
		}

	// copy and assignment
	RefCountedPtr<T,_R>(const RefCountedPtr<T,_R> & src)
	: pW( src.pW? src.pW->Copy() : NULL )
		{}
	RefCountedPtr<T,_R>& operator=(const RefCountedPtr<T,_R> & src)
		{
			if( !pW || !src.pW || (*pW != *src.pW) )
			{
				if( pW )
					pW->Release();
				pW = src.pW? src.pW->Copy() : NULL;
			}
			return *this;
		}
	T* operator=(T* src)
		{
			if( !pW || !src || (*pW != src) )
			{
				if( pW )
					pW->Release();
				pW = src? new _R( src ) : NULL;
			}
			return pW? *pW : NULL;
		}

protected:
	bool isNull() const { return (!pW || pW->isNull()); }

public:
	operator const T* () const { return pW? *pW : NULL; }
	operator T* () { return pW? *pW : NULL; }

  const T* operator->() const { return pW? *pW : NULL; }
  T* operator->() { return pW? *pW : NULL; }

	bool operator < ( const RefCountedPtr<T,_R>& Right ) const
		{
			return ((const T*)(*this) < (const T*)Right);
		}

	void Lock() { if( pW ) pW->Lock(); }
};


//interface to a ref-counted pointer that automatically constructs and returns a new T
//if it is dereferenced or operator-> is called on a NULL pointer
template< typename T >
class RefCountedAutoConstructPtr : public RefCountedPtr<T>
{
private:
	typedef RefCountedPtr<T> _base;

public:
	RefCountedAutoConstructPtr<T>() {}
	RefCountedAutoConstructPtr<T>( T* pTarget ) : _base( pTarget ) {}
	virtual ~RefCountedAutoConstructPtr<T>(void) {}

	// copy and assignment
	RefCountedAutoConstructPtr<T>(const RefCountedAutoConstructPtr<T> & src) : _base( src ) {}
	RefCountedAutoConstructPtr<T>(const _base & src) : _base( src ) {}
	RefCountedAutoConstructPtr<T>& operator=(const RefCountedAutoConstructPtr<T> & src) { _base::operator=( src ); return *this; }
	RefCountedAutoConstructPtr<T>& operator=(const _base & src) { _base::operator=( src ); return *this; }
	T* operator=(T* src) { return _base::operator=( src ); }

public:
	operator const T* () const { return isNull()? _base::operator=( new T ) : _base::operator T*(); }
	operator T* () { return isNull()? _base::operator=( new T ) : _base::operator T*(); }

  const T* operator->() const { return isNull()? _base::operator=( new T ) : _base::operator T*(); }
  T* operator->() { return isNull()? _base::operator=( new T ) : _base::operator T*(); }

	operator bool() const { return !(!pW || pW->isNull()); } //enable null testing without dereferencing
};
