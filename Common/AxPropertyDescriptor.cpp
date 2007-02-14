// AxMethodDescriptor.cpp : implementation file
//

#include "stdafx.h"
#include "AxPropertyDescriptor.h"


IMPLEMENT_SERIAL(AxPropertyDescriptor, CObject, 1)

AxPropertyDescriptor::AxPropertyDescriptor(void)
: Id( 0 ), Type( 0 ), IsArray( FALSE ), CanSet( FALSE ), NumEnum( 0 ), NumParams( 0 ), ArrEnum( NULL )
{
	CallingArgs[0] = 0;
	::memset(&Guid, 0, sizeof(GUID));
	for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
		::memset(&CallingArgClsids[i], 0, sizeof(CLSID));			
}

AxPropertyDescriptor::AxPropertyDescriptor(const AxPropertyDescriptor *other)
: Id( other->Id ), Type( other->Type ), IsArray( other->IsArray ), CanSet( other->CanSet )
, NumEnum( other->NumEnum ), NumParams( other->NumParams ), ArrEnum( NULL )
, DocumentationDesc( other->DocumentationDesc ), Guid( other->Guid ), invKind( other->invKind )
{
	ArrEnum = new AxPropertyEnum[other->NumEnum];
	for( int i = 0; i < other->NumEnum; ++i )
	{
		ArrEnum[i].Name = other->ArrEnum[i].Name;
		ArrEnum[i].Var = other->ArrEnum[i].Var;
	}
	for( int i = 0; i < MAX_CALLING_ARGUMENTS; ++i )
	{
		CallingArgs[i] = other->CallingArgs[i];
		CallingArgNames[i] = other->CallingArgNames[i];
	}
}

AxPropertyDescriptor::~AxPropertyDescriptor(void) 
{
	if (ArrEnum != NULL)
		delete [] ArrEnum;
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
