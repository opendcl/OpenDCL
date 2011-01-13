#pragma once

// This template class defines a reference counted pointer that automatically deletes the object pointed 
// to when its reference count reaches zero. Creating a copy of the pointer class increments the reference 
// count, thus ensuring that the object stays alive as long as there are any outstanding pointers to it.
//
// Usage:
// class CMyObject;
// RefCountedPtr< MyObject > pMyObject = new CMyObject;
//
// Copyright 2007 - 2008 ManuSoft. All Rights Reserved.
// http://www.manusoft.com
//
// A license to use the code in this file for the OpenDCL project has been granted
// by Owen Wengerd and ManuSoft. This code is provided under the terms of the GPL
// license governing OpenDCL. This code may not be used for any other purpose
// without the express written consent of Owen Wengerd or ManuSoft.


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
			ctRef = ~ULONG(0);
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
	T pT;

public:
	RefCounter<T>( T pTarget )
		: pT( pTarget )
		{}
	virtual ~RefCounter<T>(void)
		{ delete pT; }

private:
	// prevent copy and assignment
	RefCounter<T>(RefCounter<T> & src);
	RefCounter<T>& operator=(RefCounter<T> & src);

public:
	bool isNull() const
		{ return (pT? false : true); }

	operator const T () const
		{ return pT; }
	operator T ()
		{ return pT; }

  const T operator->() const
		{ return pT; }
  T operator->()
		{ return pT; }

	bool operator ==( const RefCounter<T>& src ) const
		{ return (*pT == *src.pT); }

	// reference counting
public:
	RefCounter<T>* Copy()
		{
			if( AddRef() == ~0 )
			{
				RefCounter<T>* pNewT = new RefCounter<T>( pT );
				pNewT->Lock();
				return pNewT;
			}
			return this;
		}

public:
	//for use by locked ref counter to prevent pT from being deleted on destruction
	T Swap( T NewTarget ) { T OldT = pT; pT = NewTarget; return OldT; }
};

template< typename T, typename R = RefCounter<T*> >
class RefCountedPtr
{
protected:
	typedef R _R;
	_R* pW;

public:
	RefCountedPtr<T,_R>()
	: pW( NULL )
		{}
	RefCountedPtr<T,_R>( T* pTarget )
	: pW( pTarget? new _R( pTarget ) : NULL )
		{}
	virtual ~RefCountedPtr<T,_R>(void)
		{
			Release();
		}

	// copy and assignment
	RefCountedPtr<T,_R>(const RefCountedPtr<T,_R> & src)
	: pW( src.pW? src.pW->Copy() : NULL )
		{}
	RefCountedPtr<T,_R>& operator=(const RefCountedPtr<T,_R> & src)
		{
			if( !pW || !src.pW || (pW != src.pW) )
			{
				Release();
				pW = src.pW? src.pW->Copy() : NULL;
			}
			return *this;
		}

private:
	void Release()
		{
			if( pW )
			{
				if( pW->Release() == ~0 )
				{
					pW->Swap( NULL );
					delete pW;
					pW = NULL;
				}
			}
		}

protected:
	bool isNull() const { return (!pW || pW->isNull()); }

public:
	operator T* () const { if( pW ) return *pW; return NULL; }
  T* operator->() const { if( pW ) return *pW; return NULL; }
	bool operator < ( const RefCountedPtr<T,_R>& Right ) const
		{
			return ((const T*)(*this) < (const T*)Right);
		}

	void Lock() { if( pW ) pW->Lock(); }
	bool isLocked()
		{
			if( !pW )
				return false;
			pW->AddRef();
			return (pW->Release() == ~0);
		}
};

template< typename T, typename R = RefCounter<T*> >
class LockedPtr : public RefCountedPtr< T, R >
{
private:
	typedef RefCountedPtr< T, R > _base;

public:
	LockedPtr( T* pTarget ) : _base( pTarget ) { Lock(); }
	virtual ~LockedPtr(void) {}
};

template< typename _RefCountedPtr >
class RefCountedPtrAsIUnknown : public IUnknown, public RefCounterBase
{
protected:
	_RefCountedPtr R;

public:
	RefCountedPtrAsIUnknown< _RefCountedPtr >( _RefCountedPtr& Src )
	: R( Src )
		{}

	operator _RefCountedPtr() { return R; }

	//IUnknown interface
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( /*[in]*/ REFIID riid,
																										/*[iid_is][out]*/ void __RPC_FAR *__RPC_FAR *ppvObject )
		{
			if( !ppvObject )
				return E_POINTER;
			if( riid == IID_IUnknown )
			{
				*ppvObject = this;
				AddRef();
				return S_OK;
			}
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}
	virtual ULONG STDMETHODCALLTYPE AddRef( void ) { return RefCounterBase::AddRef(); }
	virtual ULONG STDMETHODCALLTYPE Release( void ) { return RefCounterBase::Release(); }
};
