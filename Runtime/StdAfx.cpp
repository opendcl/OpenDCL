// (C) Copyright 2002-2005 by Autodesk, Inc.
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted,
// provided that the above copyright notice appears in all copies and
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS.
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC.
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//------ StdAfx.cpp : source file that includes just the standard includes
//------  StdAfx.pch will be the pre-compiled header
//------  StdAfx.obj will contain the pre-compiled type information
//-----------------------------------------------------------------------------
#include "StdAfx.h"


#if (_MSC_VER >= 1300) && (_MSC_VER < 1400) && (WINVER < 0x0500)
//VC7 or later, building with pre-VC7 runtime libraries
extern "C" long _ftol( double ); //defined by VC6 C libs
extern "C" long _ftol2( double dblSource ) { return _ftol( dblSource ); }
#endif

#if (_MSC_VER >= 1900)
//VC14 CRT changes result in linker errors, so define missing functions
#ifdef _WIN64
#define IMP_FPRINTF __imp_fprintf
#define IMP_IOB __imp___iob_func
#else
#define IMP_FPRINTF _imp__fprintf
#define IMP_IOB _imp___iob
#endif

extern "C" int __cdecl IMP_FPRINTF(FILE * _Stream, const char * _Format, ...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfprintf_l(_Stream, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

extern "C" FILE* __cdecl IMP_IOB(unsigned)
{
	static FILE IOB[] =
	{
		stdin,
		stdout,
		stderr
	};
	return IOB;
}
#endif

#if defined(_ARXTARGET) && (_ARXTARGET < 20)
#undef acdbHostApplicationServices

#if (_ARXTARGET == 15)
#define HOSTAPP_DLL _T("AcUtil15.dll")
#elif (_ARXTARGET == 16)
#define HOSTAPP_DLL _T("AcDb16.dll")
#elif (_ARXTARGET == 17)
#define HOSTAPP_DLL _T("AcDb17.dll")
#elif (_ARXTARGET == 18)
#define HOSTAPP_DLL _T("AcDb18.dll")
#elif (_ARXTARGET == 19)
#define HOSTAPP_DLL _T("AcDb19.dll")
#elif (_ARXTARGET == 20)
#define HOSTAPP_DLL _T("AcDb20.dll")
#endif

#ifdef HOSTAPP_DLL
AcDbHostApplicationServices* acdbHostApplicationServicesEx()
{
	static class AutoLib
	{
		HMODULE mhLib;
		AcDbHostApplicationServices** ppServices;
	public:
		AutoLib( LPCTSTR pszModule )
			: mhLib( LoadLibrary( pszModule ) )
			, ppServices( NULL )
			{
				ppServices = (AcDbHostApplicationServices**)GetProcAddress( mhLib, "s_pServices" );
			}
		~AutoLib() { FreeLibrary( mhLib ); }
		operator AcDbHostApplicationServices* () const { return (ppServices? *ppServices : NULL); }
	} theHostAppServ( HOSTAPP_DLL );
	return theHostAppServ;
}
#else
#pragma message ("     HOSTAPP_DLL should be defined in StdAfx.cpp!")
AcDbHostApplicationServices* acdbHostApplicationServicesEx() { return acdbHostApplicationServices(); } //use original
#endif

#endif //defined(_ARXTARGET) && (_ARXTARGET < 20)

