// AxEventDescriptor.cpp : implementation file
//

#include "stdafx.h"
#include "AxEventDescriptor.h"


IMPLEMENT_SERIAL(AxEventDescriptor, CObject, 1)

AxEventDescriptor:: AxEventDescriptor(void) : Id( 0 ), nArgs( 0 )
{
	CallingArgs[0] = 0; 
	CallingArgNames.SetSize(MAX_CALLING_ARGUMENTS);
	for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
		::memset(&CallingArgClsids[i], 0, sizeof(CLSID));				
}
