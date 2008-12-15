// RxInstall.cpp : Install-time AutoCAD ObjectARX module demand-load setup
//
// Copyright 2008 ManuSoft. All Rights Reserved.
// http://www.manusoft.com
//
// A license to use the code in this file for the OpenDCL project has been granted
// by Owen Wengerd and ManuSoft. This code is provided under the terms of the GPL
// license governing OpenDCL. This code may not be used for any other purpose
// without the express written consent of Owen Wengerd or ManuSoft.


#include "StdAfx.h"
#include "RegIO.h"

#ifdef _USEMSILIB
#include <msi.h>
#pragma comment(lib,"msi.lib")
#else
typedef unsigned long MSIHANDLE;
#endif

#define _elements(ARRAY) ((sizeof(ARRAY))/(sizeof(ARRAY[0])))

enum LoadReasons
{
	kOnProxyDetection       = 0x01,
	kOnAutoCADStartup       = 0x02,
	kOnCommandInvocation    = 0x04,
	kOnLoadRequest          = 0x08,
	kLoadDisabled           = 0x10,
	kTransparentlyLoadable  = 0x20
};

HMODULE ghThisModule = NULL;

const LPCTSTR REGKEY_LOADER = _T("Loader");
const LPCTSTR REGKEY_DESCRIPTION = _T("Description");
const LPCTSTR REGVAL_LOADCTRLS = _T("LoadCtrls");
const LPCTSTR REGKEY_NAME = _T("Name");
const LPCTSTR REGVAL_REGPATH = _T("RegPath");
const LPCTSTR _MODULE = _T("MODULE");
const LPCTSTR REGKEY_HKEY_LOCAL_MACHINE = _T("\\\\HKEY_LOCAL_MACHINE");
const LPCTSTR REGKEY_COMMANDS = _T("Commands");


class String
{
	TCHAR* mpszString;
public:
	// Constructors
	String() : mpszString( NULL ) {}
	String( String& Source ) : mpszString( Source.Detach() ) {}
	String( LPCTSTR pszSource ) : mpszString( StringDup( pszSource ) ) {}
	~String() { delete[] mpszString; }

	static TCHAR* StringDup( LPCTSTR pszSource )
		{
			if( !pszSource )
				return NULL;
			int cchString = lstrlen( pszSource );
			TCHAR* pszString = new TCHAR[cchString + 1];
			lstrcpyn( pszString, pszSource, cchString + 1 );
			return pszString;
		}

	String& operator=( String& Source )
		{
			delete[] mpszString;
			mpszString = Source.Detach();
			return *this;
		}

	String& operator=( LPCTSTR pszSource )
		{
			delete[] mpszString;
			mpszString = StringDup( pszSource );
			return *this;
		}

	String& operator+=( LPCTSTR pszSource )
		{
			if( pszSource && *pszSource )
			{
				int cchThis = Length();
				int cchSource = lstrlen( pszSource );
				TCHAR* pszNew = new TCHAR[cchThis + cchSource + 1];
				if( cchThis > 0 )
					lstrcpy( pszNew, mpszString );
				delete[] mpszString;
				lstrcpyn( pszNew + cchThis, pszSource, cchSource + 1 );
				mpszString = pszNew;
			}
			return *this;
		}

	String& operator+=( TCHAR chSource )
		{
			if( chSource )
			{
				int cchThis = Length();
				TCHAR* pszNew = new TCHAR[cchThis + 2];
				if( cchThis > 0 )
					lstrcpy( pszNew, mpszString );
				delete[] mpszString;
				*(pszNew + cchThis) = chSource;
				*(pszNew + cchThis + 1) = _T('\0');
				mpszString = pszNew;
			}
			return *this;
		}

	String operator+( LPCTSTR pszSource )
		{
			String sNew = *this;
			sNew += pszSource;
			return sNew;
		}

	String operator+( TCHAR chSource )
		{
			String sNew = *this;
			sNew += chSource;
			return sNew;
		}

	int Length() const { return mpszString? lstrlen( mpszString ) : 0; }
	bool IsEmpty() const { return (Length() == 0); }
	String& MakeLower() { if( mpszString ) CharLower( mpszString ); return *this; }
	LPTSTR GetBuffer( int cchBuffer ) //cheap imitation with no locking
		{
			delete[] mpszString;
			mpszString = new TCHAR[cchBuffer + 1];
			return mpszString;
		}
	String& ReleaseBuffer( int cchBuffer = -1 ) //no range checking!
		{
			if( !mpszString )
				return *this;
			if( cchBuffer >= 0 )
				mpszString[cchBuffer] = _T('\0');
			return *this;
		}
	String Left( int cchBuffer ) const
		{
			if( !mpszString )
				return String();
			int cchThis = lstrlen( mpszString );
			if( cchBuffer > cchThis )
				cchBuffer = cchThis;
			String sResult;
			lstrcpyn( sResult.GetBuffer( cchBuffer ), mpszString, cchBuffer );
			sResult.ReleaseBuffer( cchBuffer );
			return sResult;
		}
	String Right( int cchBuffer ) const
		{
			if( !mpszString )
				return String();
			int cchThis = lstrlen( mpszString );
			if( cchBuffer > cchThis )
				cchBuffer = cchThis;
			String sResult;
			lstrcpyn( sResult.GetBuffer( cchBuffer ), mpszString + cchThis - cchBuffer, cchBuffer );
			sResult.ReleaseBuffer( cchBuffer );
			return sResult;
		}
	int Find( LPCTSTR pszTarget ) const
		{
			if( !mpszString || !*mpszString || !pszTarget || !*pszTarget )
				return -1;
			int cchThis = lstrlen( mpszString );
			int cchTarget = lstrlen( pszTarget );
			if( cchTarget > cchThis )
				return -1;
			int idxStop = cchThis - cchTarget;
			for( int idx = 0; idx <= idxStop; ++idx )
			{
				if( mpszString[idx] == *pszTarget && lstrcmp( &mpszString[idx], pszTarget ) == 0 )
					return idx;
			}
			return -1;
		}
	int Find( TCHAR chTarget ) const
		{
			if( !mpszString || !*mpszString )
				return -1;
			int cchThis = lstrlen( mpszString );
			for( int idx = 0; idx < cchThis; ++idx )
			{
				if( mpszString[idx] == chTarget )
					return idx;
			}
			return -1;
		}
	String SpanExcluding( LPCTSTR pszExclude ) const
		{
			if( !mpszString )
				return String();
			if( !pszExclude || !*pszExclude )
				return String( mpszString );
			String sExclude = pszExclude;
			int cchThis = lstrlen( mpszString );
			for( int idx = 0; idx < cchThis; ++idx )
			{
				TCHAR chThis = mpszString[idx];
				if( sExclude.Find( chThis ) >= 0 )
					return Left( idx - 1 );
			}
			return String( mpszString );
		}
	TCHAR* Detach()
		{
			TCHAR* pszString = mpszString;
			mpszString = NULL;
			return pszString;
		}
	operator LPCTSTR() const { return mpszString; }
};

UINT GetRxCommandArray( LPCTSTR*& rpszCommands )
{
	static LPCTSTR     _OPENDCL = _T("OPENDCL");

	static LPCTSTR rszCommands[] =
	{
		_OPENDCL,
	};
	rpszCommands = rszCommands;
	return _elements(rszCommands);
}


LPCTSTR GetMfrName()
{
	return NULL;
}


LPCTSTR GetAppName()
{
	return _T("OpenDCL");
}


LPCTSTR GetAppLongName()
{
	return GetAppName();
}


bool GetModulePath( HMODULE hmodTarget, LPTSTR pszPath, DWORD& cchPath )
{
	cchPath = ::GetModuleFileName( hmodTarget, pszPath, cchPath );
	return (cchPath != 0);
}


bool GetModuleDirectory( HMODULE hmodTarget, LPTSTR pszPath, DWORD& cchPath )
{
	DWORD cchBuf = cchPath;
	if( !GetModulePath( hmodTarget, pszPath, cchBuf ) )
		return false;
	while( cchBuf > 0 )
	{
		--cchBuf;
		if( pszPath[cchBuf] == _T('\\') ||
				pszPath[cchBuf] == _T(':') ||
				pszPath[cchBuf] == _T('/') )
		{
			pszPath[++cchBuf] = _T('\0');
			cchPath = cchBuf;
			return true;
		}
	}
	cchPath = 0;
	return true;
}


String GetThisModuleDirectory()
{
	String sModuleDirectory;
	DWORD cchModuleDirectory = MAX_PATH;
	if( !GetModuleDirectory( ghThisModule,
													 sModuleDirectory.GetBuffer( cchModuleDirectory + 1 ),
													 cchModuleDirectory ) )
		return NULL;
	return sModuleDirectory.ReleaseBuffer();
}


String GetSiblingModulePath( LPCTSTR pszFilename )
{
	return (GetThisModuleDirectory() + pszFilename);
}


LPCTSTR GetTargetModulePath( UINT nAcadTarget, LPCTSTR pszModifier = NULL )
{
	TCHAR szAcadTarget[32];
	_ultot_s( nAcadTarget, szAcadTarget, 10 );
	String sAppFilename = GetAppName();
	if( sAppFilename.IsEmpty() )
		sAppFilename = _T("OpenDCL");
	if( pszModifier )
	{
		sAppFilename += _T(".");
		sAppFilename += pszModifier;
	}
	sAppFilename += _T(".");
	sAppFilename += szAcadTarget;
	sAppFilename += _T(".arx");
	static String sPath;
	sPath = GetSiblingModulePath( sAppFilename );
	return sPath;
}


typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
bool IsWow64()
{
#ifdef _WIN64
	return false;
#else
  static BOOL bIsWow64 = FALSE;
	static bool bAlreadyTested = false;
	if( !bAlreadyTested )
	{
		LPFN_ISWOW64PROCESS fnIsWow64Process =
			(LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle( _T("KERNEL32") ), "IsWow64Process" );
    if( fnIsWow64Process )
			fnIsWow64Process( GetCurrentProcess(), &bIsWow64 );
		bAlreadyTested = true;
	}
	return (bIsWow64 != FALSE);
#endif //_WIN64
}


BOOL APIENTRY DllMain( HMODULE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls( hModule );
			ghThisModule = hModule;
			break;
		}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
    return TRUE;
}


void CreateDemandLoadEntry( UINT nAcadTarget, RegKey& rkDemandLoad, LPCTSTR pszModifier = NULL )
{
	if( !rkDemandLoad )
		return;
	if( nAcadTarget >= 16 )
	{
		rkDemandLoad.SetValue( REGKEY_LOADER, GetTargetModulePath( nAcadTarget, pszModifier ) );
		rkDemandLoad.SetValue( REGKEY_DESCRIPTION, GetAppLongName() );
		if( !rkDemandLoad.HasValue( REGVAL_LOADCTRLS ) )
			rkDemandLoad.SetValue( REGVAL_LOADCTRLS, kOnCommandInvocation | kOnLoadRequest/* | kOnAutoCADStartup*/ );
		{
			RegKey rkCommands( REGKEY_COMMANDS, rkDemandLoad, true, KEY_WRITE );
			LPCTSTR* rpszCommands = NULL;
			for( int idx = GetRxCommandArray( rpszCommands ) - 1; idx >= 0; --idx )
				rkCommands.SetValue( rpszCommands[idx], rpszCommands[idx] );
		}
	}
	else
	{
		LPCTSTR pszKey = rkDemandLoad.Key();
		size_t cchHKLM = lstrlen( REGKEY_HKEY_LOCAL_MACHINE );
		TCHAR* pszRegPath = new TCHAR[cchHKLM + 1 + lstrlen( pszKey ) + 1];
		lstrcpy( pszRegPath, REGKEY_HKEY_LOCAL_MACHINE );
		pszRegPath[cchHKLM] = _T('\\');
		lstrcpy( pszRegPath + cchHKLM + 1, pszKey );
		rkDemandLoad.SetValue( REGVAL_REGPATH, pszRegPath );
		delete [] pszRegPath;
		if( !rkDemandLoad.HasValue( REGVAL_LOADCTRLS ) )
			rkDemandLoad.SetValue( REGVAL_LOADCTRLS, kOnCommandInvocation | kOnLoadRequest/* | kOnAutoCADStartup*/ );
		{
			RegKey rkLoader( REGKEY_LOADER, rkDemandLoad, true, KEY_WRITE );
			rkLoader.SetValue( _MODULE, GetTargetModulePath( nAcadTarget, pszModifier ) );
		}
		{
			RegKey rkName( REGKEY_NAME, rkDemandLoad, true, KEY_WRITE );
			rkName.SetValue( GetAppLongName(), GetAppLongName() );
		}
		{
			RegKey rkCommands( REGKEY_COMMANDS, rkDemandLoad, true, KEY_WRITE );
			LPCTSTR* rpszCommands = NULL;
			for( int idx = GetRxCommandArray( rpszCommands ) - 1; idx >= 0; --idx )
				rkCommands.SetValue( rpszCommands[idx], rpszCommands[idx] );
		}
	}
}


HKEY GetRootRegKey( UINT nAcadTarget, bool bHKLM = true )
{
	if( nAcadTarget < 16 )
		return HKEY_LOCAL_MACHINE;
	static bool bHKLMAccessible = !!RegKey( _T("SOFTWARE"), HKEY_LOCAL_MACHINE );
	return (bHKLM && bHKLMAccessible)? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
}


void RxSelfInstallImp( UINT nAcadTarget, LPCTSTR pszTargetKey, bool bHKLM = true, bool bX64 = false )
{
	RegKey rkRoot( pszTargetKey, HKEY_LOCAL_MACHINE, false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) );
	RegKey rkDemandLoad( String( pszTargetKey ) + _T("\\Applications\\") + GetAppLongName(),
											 GetRootRegKey( nAcadTarget, bHKLM ),
											 true,
											 KEY_WRITE | (bX64? KEY_WOW64_64KEY : 0) );
	if( !rkDemandLoad )
		return;
	CreateDemandLoadEntry( nAcadTarget, rkDemandLoad, (bX64? _T("x64") : NULL) );
}


bool EnumerateRegTargets( UINT nAcadTarget, LPCTSTR pszRegKey, bool bHKLM = true, bool bX64 = false )
{
	String sAcadRootKey = _T("SOFTWARE\\Autodesk\\AutoCAD\\");
	sAcadRootKey += pszRegKey;
	RegKey rkTarget( sAcadRootKey, GetRootRegKey( nAcadTarget, bHKLM ), false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) );
	if( !rkTarget )
		return false;

	DWORD cchMaxSubkey = MAX_PATH;
	RegQueryInfoKey( rkTarget, NULL, NULL, NULL, NULL, &cchMaxSubkey, NULL, NULL, NULL, NULL, NULL, NULL );
	DWORD dwBufSize = cchMaxSubkey + 1;
	String sSubkey;
	DWORD idxSubkey = 0;
	while( RegEnumKeyEx( rkTarget,
												idxSubkey,
												sSubkey.GetBuffer( dwBufSize ),
												&dwBufSize,
												NULL,
												NULL,
												NULL,
												NULL ) == ERROR_SUCCESS )
	{
		idxSubkey++;
		sSubkey.ReleaseBuffer();
		String sAcadTargetKey( (LPCTSTR)sAcadRootKey );
		sAcadTargetKey += _T('\\');
		sAcadTargetKey += sSubkey;
		RxSelfInstallImp( nAcadTarget, sAcadTargetKey, bHKLM, bX64 );
		dwBufSize = cchMaxSubkey + 1;
	}
	return true;
}

extern "C"
__declspec(dllexport)
UINT __stdcall RxInstall( MSIHANDLE hInstall )
{
#ifdef _USEMSILIB
	TCHAR szCustomData[64] = _T("");
	DWORD cchCustomData = sizeof(szCustomData) / sizeof(szCustomData[0]);
	MsiGetProperty( hInstall, _T("CustomData"), szCustomData, &cchCustomData );
	bool bHKLM = (lstrcmpi( szCustomData, _T("HKLM") ) == 0);
#else
	bool bHKLM = true;
#endif
	//EnumerateRegTargets( 14, _T("R14.0"), true );
	//EnumerateRegTargets( 15, _T("R15.0"), true );
	EnumerateRegTargets( 16, _T("R16.0"), bHKLM );
	EnumerateRegTargets( 16, _T("R16.1"), bHKLM );
	EnumerateRegTargets( 16, _T("R16.2"), bHKLM );
	EnumerateRegTargets( 17, _T("R17.0"), bHKLM );
	EnumerateRegTargets( 17, _T("R17.1"), bHKLM );
	EnumerateRegTargets( 17, _T("R17.2"), bHKLM );
	if( IsWow64() )
	{
		EnumerateRegTargets( 17, _T("R17.1"), bHKLM, true );
		EnumerateRegTargets( 17, _T("R17.2"), bHKLM, true );
	}
	return ERROR_SUCCESS;
}


bool RemoveAllRegTargets( LPCTSTR pszRegKey, HKEY hkRoot, bool bX64 = false )
{
	TCHAR szAdeskRoot[] = _T("SOFTWARE\\Autodesk");
	RegKey rkAdesk( szAdeskRoot, hkRoot, false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) );
	RegKey rkTarget( pszRegKey, rkAdesk, false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) );
	if( !rkTarget )
		return false;

	DWORD cchMaxSubkey = MAX_PATH;
	RegQueryInfoKey( rkTarget, NULL, NULL, NULL, NULL, &cchMaxSubkey, NULL, NULL, NULL, NULL, NULL, NULL );
	TCHAR* pszSubkey = new TCHAR[cchMaxSubkey + 1];
	DWORD idxSubkey = 0;
	DWORD dwBufSize = cchMaxSubkey + 1;
	while( RegEnumKeyEx( rkTarget,
												idxSubkey,
												pszSubkey,
												&dwBufSize,
												NULL,
												NULL,
												NULL,
												NULL ) == ERROR_SUCCESS )
	{
		idxSubkey++;
		RegKey rkTargetVer( pszSubkey, rkTarget, false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) );
		RegKey rkApps( _T("Applications"), rkTargetVer, false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) );
		RegKey rkThisApp( GetAppLongName(), rkApps, false, KEY_ALL_ACCESS | (bX64? KEY_WOW64_64KEY : 0) );
		if( rkThisApp )
			RegRemoveKey( rkThisApp );
		dwBufSize = cchMaxSubkey + 1;
	}
	delete[] pszSubkey;
	return true;
}

extern "C"
__declspec(dllexport)
UINT __stdcall RxUninstall( MSIHANDLE hInstall )
{
#ifdef _USEMSILIB
	TCHAR szCustomData[64] = _T("");
	DWORD cchCustomData = sizeof(szCustomData) / sizeof(szCustomData[0]);
	MsiGetProperty( hInstall, _T("CustomData"), szCustomData, &cchCustomData );
	bool bHKLM = (lstrcmpi( szCustomData, _T("HKLM") ) == 0);
#else
	bool bHKLM = true;
#endif
	//RemoveAllRegTargets( _T("AutoCAD\\R14.0"), HKEY_LOCAL_MACHINE );
	//RemoveAllRegTargets( _T("AutoCAD\\R15.0"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("AutoCAD\\R16.0"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("AutoCAD\\R16.0"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("AutoCAD\\R16.1"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("AutoCAD\\R16.1"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("AutoCAD\\R16.2"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("AutoCAD\\R16.2"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("AutoCAD\\R17.0"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("AutoCAD\\R17.0"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("AutoCAD\\R17.1"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("AutoCAD\\R17.1"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("AutoCAD\\R17.2"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("AutoCAD\\R17.2"), HKEY_CURRENT_USER );
	if( IsWow64() )
	{
		RemoveAllRegTargets( _T("AutoCAD\\R17.1"), HKEY_LOCAL_MACHINE, true );
		RemoveAllRegTargets( _T("AutoCAD\\R17.1"), HKEY_CURRENT_USER, true );
		RemoveAllRegTargets( _T("AutoCAD\\R17.2"), HKEY_LOCAL_MACHINE, true );
		RemoveAllRegTargets( _T("AutoCAD\\R17.2"), HKEY_CURRENT_USER, true );
	}
	return ERROR_SUCCESS;
}
