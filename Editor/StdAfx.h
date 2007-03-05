// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#pragma warning(disable: 4995 4355)

#define STRICT

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

//- Modify the following defines if you have to target a platform prior to the ones specified below.
//- Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER					//- Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400			//- Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT			//- Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400		//- Change this to the appropriate value to target Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS			//- Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410	//- Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE				//- Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400		//- Change this to the appropriate value to target IE 5.0 or later.
#endif


#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#include <AFXDTCTL.H>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


// activeX includes and import
#include <afxpriv.h>

#include <afxtempl.h>
#include <iostream>
#include <afxctl.h>
#include <afxconv.h>
#include <comcat.h>
#include <windowsx.h>
#include <activscp.h>
#include <objsafe.h>
#include <shlwapi.h>
#include <atlbase.h>
#include <atlconv.h>
#include <comutil.h>


#pragma warning( disable: 4310 )  // Cast truncates constant value
#pragma warning( disable: 4244 )  // Conversion: possible loss of data
#pragma warning( disable: 4099 )  // class/struct confusion


#include <comdef.h>

#pragma warning( default: 4310 )  // Cast truncates constant value
#pragma warning( default: 4244 )  // Conversion: possible loss of data
#pragma warning( default: 4099 )  // class/struct confusion


#include <assert.h>

#include "RefCountedPtr.h"


#define _elements(ARRAY) ((sizeof(ARRAY))/(sizeof(ARRAY[0])))
#define _stringify(x) #x
#define _stringify_value(x) _stringify(x)
#define _L_token(s) L ## s
#undef _L
#define _L(s) _L_token(s)


//Include diagnostic functionality in all build configurations by default
#ifndef _NO_DIAGNOSTIC
#define _DIAGNOSTIC
#include "Diagnostic.h"
#endif //_NO_DIAGNOSTIC


//-----------------------------------------------------------------------------
//----- Debugging helpers
#ifdef _DEBUG

inline void _cdecl Trace( LPCTSTR pszTraceOutput ) { ::OutputDebugString( pszTraceOutput ); }
inline void _cdecl TraceFmt( LPCTSTR pszTraceOutputFmt, ... )
	{
		TCHAR szBuf[1024];
		va_list args;
		va_start( args, pszTraceOutputFmt );
		_vsntprintf( szBuf, _elements(szBuf) - 1, pszTraceOutputFmt, args );
		::OutputDebugString( szBuf );
	}
#else
inline void _cdecl Trace( LPCTSTR ) {}
inline void _cdecl TraceFmt( LPCTSTR, ... ) {}
#endif //_DEBUG
