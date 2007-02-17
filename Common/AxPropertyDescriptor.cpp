// AxMethodDescriptor.cpp : implementation file
//

#include "stdafx.h"
#include "AxPropertyDescriptor.h"


IMPLEMENT_SERIAL(AxPropertyDescriptor, CObject, 1)

AxPropertyDescriptor::AxPropertyDescriptor(void)
: Id( 0 ), Type( 0 ), IsArray( FALSE ), CanSet( FALSE )
{
	::memset(&Guid, 0, sizeof(GUID));
}

AxPropertyDescriptor::AxPropertyDescriptor(const AxPropertyDescriptor *other)
: Id( other->Id ), Type( other->Type ), IsArray( other->IsArray ), CanSet( other->CanSet )
, DocumentationDesc( other->DocumentationDesc ), Guid( other->Guid ), invKind( other->invKind )
, rArgs( other->rArgs ), rEnum( other->rEnum )
{
}

AxPropertyDescriptor::~AxPropertyDescriptor(void) 
{
}

#ifdef _DIAGNOSTIC
LPCTSTR AxPropertyDescriptor::toString() const
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("{DISPID: %s, \"%s\", \"%s\"}"),
							asString( Id ), (LPCTSTR)Name, (LPCTSTR)DocumentationDesc );
	return buf;
}
#endif
