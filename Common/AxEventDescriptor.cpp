// AxEventDescriptor.cpp : implementation file
//

#include "stdafx.h"
#include "AxEventDescriptor.h"


IMPLEMENT_SERIAL(AxEventDescriptor, CObject, 1)

AxEventDescriptor::AxEventDescriptor(void) : Id( 0 ), nArgs( 0 )
{
	CallingArgs[0] = 0; 
	CallingArgNames.SetSize(MAX_CALLING_ARGUMENTS);
	for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
		::memset(&CallingArgClsids[i], 0, sizeof(CLSID));				
}

AxEventDescriptor::~AxEventDescriptor(void)
{
}


#ifdef _DIAGNOSTIC
LPCTSTR AxEventDescriptor::toString() const
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("{DISPID: %s, \"%s\", \"%s\"}"),
							asString( Id ), (LPCTSTR)Name, (LPCTSTR)DocumentationDesc );
	return buf;
}
#endif
