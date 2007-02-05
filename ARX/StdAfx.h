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
//- StdAfx.h : include file for standard system include files,
//-      or project specific include files that are used frequently,
//-      but are changed infrequently
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
//- 'DEBUG workaround' below prevents the MFC or ATL #include-s from pulling 
//- in "Afx.h" that would force the debug CRT through #pragma-s.
#if defined(_DEBUG) && !defined(AC_FULL_DEBUG)
#define _DEBUG_WAS_DEFINED
#undef _DEBUG
#pragma message ("     Compiling MFC / STL / ATL header files in release mode.")
#endif

#pragma pack (push, 8)
#pragma warning(disable: 4786 4996)
//#pragma warning(disable: 4098)

//-----------------------------------------------------------------------------
#define STRICT

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN			//- Exclude rarely-used stuff from Windows headers
#endif

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


//- ObjectARX and OMF headers needs this
#include <map>

//-----------------------------------------------------------------------------
#include <afxwin.h>				//- MFC core and standard components
#include <afxext.h>				//- MFC extensions
#include <afxtempl.h>     //- MFC templated container classes
#include <afxctl.h>       //- MFC support for ActiveX Controls

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>				//- MFC OLE classes
#include <afxodlgs.h>			//- MFC OLE dialog classes
#include <afxdisp.h>			//- MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>				//- MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>				//- MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>			//- MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>				//- MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//-----------------------------------------------------------------------------
#include <iostream>
#include <fstream>

//-----------------------------------------------------------------------------
#include <atlimage.h>

//-----------------------------------------------------------------------------
//- Include ObjectDBX/ObjectARX headers
//- Uncomment one of the following lines to bring a given library in your project.
//#define _BREP_SUPPORT_			//- Support for the BRep API
//#define _HLR_SUPPORT_				//- Support for the Hidden Line Removal API
//#define _AMODELER_SUPPORT_		//- Support for the AModeler API
//#define _ASE_SUPPORT_				//- Support for the ASI/ASE API
//#define _RENDER_SUPPORT_			//- Support for the AutoCAD Render API
//#define _ARX_CUSTOM_DRAG_N_DROP_	//- Support for the ObjectARX Drag'n Drop API
//#define _INC_LEAGACY_HEADERS_		//- Include legacy headers in this project
#include "ARXVI.h" //include AutoCAD target version independence header before ObjectARX headers
#include "arxHeaders.h"

//-----------------------------------------------------------------------------
#include "DocData.h" //- Your document specific data class holder

//- Declare it as an extern here so that it becomes available in all modules
extern AcApDataManager<CDocData> DocVars ;

#pragma pack (pop)

//-----------------------------------------------------------------------------
#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG
#undef _DEBUG_WAS_DEFINED
#endif

#include "RefCountedPtr.h"

#define _elements(ARRAY) ((sizeof(ARRAY))/(sizeof(ARRAY[0])))
#define _stringify(x) #x
#define _stringify_value(x) _stringify(x)
#define _L_token(s) L ## s
#undef _L
#define _L(s) _L_token(s)

//ACHAR typedefs
typedef ACHAR* LPACRXSTR;
typedef const ACHAR* LPCACRXSTR;

extern HMODULE appResModule(void); //get the localized resource module

//We need a custom resource override function that uses AfxSetResourceHandle() instead of
//AcApDocManager::pushResourceHandle() because the latter crashes in a zero-doc state
class CAcAppContextModuleResourceOverride
{
	HMODULE m_hmodOriginal;
	bool mbMustRestore;
public:
	CAcAppContextModuleResourceOverride()
		:	m_hmodOriginal( AfxGetResourceHandle() ), mbMustRestore( true )
	{
		AfxSetResourceHandle( appResModule() );
	}
	CAcAppContextModuleResourceOverride( HMODULE hmodNew )
		:	m_hmodOriginal( AfxGetResourceHandle() ), mbMustRestore( true )
	{
		AfxSetResourceHandle( hmodNew );
	}
	~CAcAppContextModuleResourceOverride()
	{
		restore();
	}
	void restore()
	{
		if( mbMustRestore )
		{
			AfxSetResourceHandle( m_hmodOriginal );
			mbMustRestore = false;
		}
	}
};


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


//some gymnastics to prevent implicit import of s_pServices
#define acdbHostApplicationServices acdbHostApplicationServicesEx
extern AcDbHostApplicationServices* acdbHostApplicationServicesEx();
