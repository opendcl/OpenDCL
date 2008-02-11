// LispIO.h : header file
//

#pragma once


namespace odcl
{
	enum PtrType
	{
		ptrHandle,
		ptrDclProject,
		ptrDclControl,
		ptrIUnknown,
		ptrBinFile,
	};
};


inline void acedRetPointer( void* ptr, odcl::PtrType type )
{
	struct resbuf rbRet = {NULL,RTENAME};
	rbRet.resval.rlname[0] = (LONG_PTR)ptr;
	rbRet.resval.rlname[1] = type;
	acedRetVal( &rbRet );
}


inline void acedRetLong( LONG lValue )
{
	struct resbuf rbRet = {NULL,RTLONG};
	rbRet.resval.rlong = lValue;
	acedRetVal( &rbRet );
}


inline void acedRetHandle( DWORD_PTR hdl )
{
	if( hdl <= LONG_MAX )
		acedRetLong( (long)hdl );
	else
		acedRetPointer( (void*)hdl, odcl::ptrHandle );
}


inline void acedRetIUnknown( IUnknown* pUnknown )
{
	if( !pUnknown )
		acedRetNil();
	else
		acedRetPointer( pUnknown, odcl::ptrIUnknown );
}
