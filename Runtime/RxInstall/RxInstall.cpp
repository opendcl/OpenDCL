// RxInstall.cpp : Install-time AutoCAD ObjectARX module demand-load setup
//
// Copyright 2010 ManuSoft. All Rights Reserved.
// http://www.manusoft.com
//
// A license to use the code in this file for the OpenDCL project has been granted
// by Owen Wengerd and ManuSoft. This code is provided under the terms of the GPL
// license governing OpenDCL. This code may not be used for any other purpose
// without the express written consent of Owen Wengerd or ManuSoft.


#include "StdAfx.h"
#include "RegIO.h"

#define _USEMSILIB

#ifdef _USEMSILIB
#include <msiquery.h>
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
	kTransparentlyLoadable  = 0x20,
	kOnIdleLoad             = 0x40,
};

static HMODULE ghThisModule = NULL;

static const LPCTSTR REGKEY_LOADER = _T("Loader");
static const LPCTSTR REGKEY_DESCRIPTION = _T("Description");
static const LPCTSTR REGVAL_LOADCTRLS = _T("LoadCtrls");
static const LPCTSTR REGKEY_NAME = _T("Name");
static const LPCTSTR REGVAL_REGPATH = _T("RegPath");
static const LPCTSTR _MODULE = _T("MODULE");
static const LPCTSTR REGKEY_HKEY_LOCAL_MACHINE = _T("\\\\HKEY_LOCAL_MACHINE");
static const LPCTSTR REGKEY_COMMANDS = _T("Commands");


class String
{
	TCHAR* mpszString;
public:
	// Constructors
	String() : mpszString( NULL ) {}
	String( String& Source ) : mpszString( Source.Detach() ) {}
	String( LPCTSTR pszSource ) : mpszString( StringDup( pszSource ) ) {}
	String( long lSource ) : mpszString( NULL )
		{
			(*this) = lSource;
		}
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

	String& operator=( long lSource )
		{
			delete[] mpszString;
			GetBuffer( 63 );
			_ltot_s( lSource, mpszString, 63, 10 );
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
			lstrcpyn( sResult.GetBuffer( cchBuffer ), mpszString, cchBuffer + 1 );
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
			lstrcpyn( sResult.GetBuffer( cchBuffer ), mpszString + cchThis - cchBuffer, cchBuffer + 1 );
			sResult.ReleaseBuffer( cchBuffer );
			return sResult;
		}
	String Mid( int cchStart, int cchLength = -1 ) const
		{
			if( !mpszString )
				return String();
			int cchThis = lstrlen( mpszString );
			if( cchStart >= cchThis )
				return String();
			if( cchLength < 0 || (cchStart + cchLength > cchThis) )
				cchLength = cchThis - cchStart;
			String sResult;
			lstrcpyn( sResult.GetBuffer( cchLength ), mpszString + cchStart, cchLength + 1 );
			sResult.ReleaseBuffer( cchLength );
			return sResult;
		}
	String Tokenize( TCHAR chToken, int& idxStart ) const
		{
			if( mpszString )
			{
				int cchThis = lstrlen( mpszString );
				if( idxStart < cchThis )
				{
					LPCTSTR pszCursor = mpszString + idxStart;
					int idxFirst = idxStart;
					while( idxStart++ < cchThis && *pszCursor != chToken )
						++pszCursor;
					int cchToken = idxStart - idxFirst - 1;
					if( idxStart >= cchThis )
						idxStart = -1;
					String sResult;
					lstrcpyn( sResult.GetBuffer( cchToken ), mpszString + idxFirst, cchToken + 1 );
					sResult.ReleaseBuffer( cchToken );
					return sResult;
				}
			}
			idxStart = -1;
			return String();
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
					return Left( idx );
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
	long ToLong() const { return (mpszString? _tstol( mpszString ) : 0); }
	bool operator==( LPCTSTR pszCompare ) const
		{
			if( !pszCompare || !*pszCompare )
				return (!mpszString || !*mpszString);
			if( !mpszString )
				return false;
			return (lstrcmp( mpszString, pszCompare ) == 0);
		}
	TCHAR operator[]( int idxCh ) const
		{
			if( idxCh < 0 )
				return NULL;
			if( !mpszString )
				return NULL;
			int cchThis = lstrlen( mpszString );
			if( idxCh >= cchThis )
				return NULL;
			return mpszString[idxCh];
		}
};

static UINT GetRxCommandArray( LPCTSTR*& rpszCommands )
{
	static LPCTSTR rszCommands[] =
	{
		_T("OPENDCL"),
	};
	rpszCommands = rszCommands;
	return _elements(rszCommands);
}


static LPCTSTR GetMfrName()
{
	return NULL;
}


static LPCTSTR GetAppName()
{
	return _T("OpenDCL");
}


static LPCTSTR GetAppLongName()
{
	return GetAppName();
}


typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
static bool IsWow64()
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
			DisableThreadLibraryCalls( hModule );
			ghThisModule = hModule;
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
  return TRUE;
}


class TargetModule
{
public:
	enum Platform { kNone = 0, kAutoCAD, kBricscad, };
	enum MajorVersion { kBRX9 = 9, kBRX10 = 10, kBRX11 = 11, kARX1 = 13,
											kARX2 = 14, kARX2000 = 15, kARX2004 = 16,
											kARX2007 = 17, kARX2010 = 18, };
	typedef char TByte;
	TargetModule( Platform platform, MajorVersion majorVer, TByte minorVer, LPCTSTR pszInstallDir = NULL )
		: mPlatform( platform )
		, mMajorVer( majorVer )
		, mMinorVer( minorVer )
		, msInstallDir( pszInstallDir )
		{}
	TargetModule( unsigned int target, LPCTSTR pszInstallDir = NULL )
		: mPlatform( static_cast< Platform >((TByte)(target >> shftPlatform)) )
		, mMajorVer( static_cast< MajorVersion >((TByte)(target >> shftMajorVer)) )
		, mMinorVer( (TByte)target )
		, msInstallDir( pszInstallDir )
		{}
	TargetModule( const TargetModule& Src )
		: mPlatform( Src.mPlatform )
		, mMajorVer( Src.mMajorVer )
		, mMinorVer( Src.mMinorVer )
		, msInstallDir( (LPCTSTR)Src.msInstallDir )
		{}
	operator unsigned int() const { return ((mPlatform << sizeof(shftPlatform)) | (mMajorVer << sizeof(shftMajorVer)) | mMinorVer); }
	Platform platform() const { return static_cast< Platform >(mPlatform); }
	MajorVersion majorVersion() const { return static_cast< MajorVersion >(mMajorVer); }
	TByte minorVersion() const { return mMinorVer; }
	LPCTSTR installDir() const { return msInstallDir; }
protected:
	TByte mPlatform;
	TByte mMajorVer;
	TByte mMinorVer;
	String msInstallDir;
	static const int shftPlatform = ((sizeof(TByte) + sizeof(TByte)) << 3);
	static const int shftMajorVer = (sizeof(TByte) << 3);
	static bool isHKLMAccessible()
		{
			static bool bHKLMAccessible = !!RegKey( _T("SOFTWARE"), HKEY_LOCAL_MACHINE );
			return bHKLMAccessible;
		}
	static String GetModulePath( HMODULE hmodTarget )
		{
			String sPath;
			DWORD cchPath = ::GetModuleFileName( hmodTarget, sPath.GetBuffer( MAX_PATH ), MAX_PATH );
			sPath.ReleaseBuffer( cchPath );
			return sPath;
		}
	static String GetModuleDirectory( HMODULE hmodTarget )
		{
			String sPath = GetModulePath( hmodTarget );
			int cchBuf = sPath.Length();
			while( --cchBuf >= 0 )
			{
				if( sPath[cchBuf] == _T('\\') ||
						sPath[cchBuf] == _T(':') ||
						sPath[cchBuf] == _T('/') )
				{
					sPath.ReleaseBuffer( cchBuf + 1 );
					break;
				}
			}
			return sPath;
		}
	String GetTargetModuleDirectory() const
		{
			String sPath = installDir();
			if( sPath.IsEmpty() )
				return GetModuleDirectory( ghThisModule );
			if( !sPath.Right( 1 ).SpanExcluding( _T("\\/") ).IsEmpty() )
				sPath += _T('\\');
			return sPath;
		}
	String GetSiblingModulePath( LPCTSTR pszFilename ) const
		{
			return (GetTargetModuleDirectory() + pszFilename);
		}
public:
	//shortcuts
	enum Target
	{
		kAutoCAD14 =    ((kAutoCAD << shftPlatform) | (kARX2 << shftMajorVer) | 0),
		kAutoCAD2000 =  ((kAutoCAD << shftPlatform) | (kARX2000 << shftMajorVer) | 0),
		kAutoCAD2000i = ((kAutoCAD << shftPlatform) | (kARX2000 << shftMajorVer) | 0),
		kAutoCAD2002 =  ((kAutoCAD << shftPlatform) | (kARX2000 << shftMajorVer) | 0),
		kAutoCAD2004 =  ((kAutoCAD << shftPlatform) | (kARX2004 << shftMajorVer) | 0),
		kAutoCAD2005 =  ((kAutoCAD << shftPlatform) | (kARX2004 << shftMajorVer) | 1),
		kAutoCAD2006 =  ((kAutoCAD << shftPlatform) | (kARX2004 << shftMajorVer) | 2),
		kAutoCAD2007 =  ((kAutoCAD << shftPlatform) | (kARX2007 << shftMajorVer) | 0),
		kAutoCAD2008 =  ((kAutoCAD << shftPlatform) | (kARX2007 << shftMajorVer) | 1),
		kAutoCAD2009 =  ((kAutoCAD << shftPlatform) | (kARX2007 << shftMajorVer) | 2),
		kAutoCAD2010 =  ((kAutoCAD << shftPlatform) | (kARX2010 << shftMajorVer) | 0),
		kAutoCAD2011 =  ((kAutoCAD << shftPlatform) | (kARX2010 << shftMajorVer) | 1),
		kAutoCAD2012 =  ((kAutoCAD << shftPlatform) | (kARX2010 << shftMajorVer) | 2),
		kBricscad9_3 =  ((kBricscad << shftPlatform) | (kBRX9 << shftMajorVer) | 3),
		kBricscad10 =  ((kBricscad << shftPlatform) | (kBRX10 << shftMajorVer) | 0),
		kBricscad11 =  ((kBricscad << shftPlatform) | (kBRX11 << shftMajorVer) | 0),
	};

	HKEY GetTargetRootRegKey( bool bWantHKLM = true ) const
		{
			if( platform() == kAutoCAD && majorVersion() < kARX2004 )
				return (isHKLMAccessible()? HKEY_LOCAL_MACHINE : NULL);
			if( !bWantHKLM )
				return HKEY_CURRENT_USER;
			return (isHKLMAccessible()? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER);
		}
	String GetTargetAppRegKey() const
		{
			String sRootKey;
			switch( platform() )
			{
			case kAutoCAD:
				{
					sRootKey = _T("SOFTWARE\\Autodesk\\AutoCAD\\R");
					String sMajor( majorVersion() );
					String sMinor( minorVersion() );
					sRootKey += sMajor;
					sRootKey += _T('.');
					sRootKey += sMinor;
				}
				break;
			case kBricscad:
				sRootKey = _T("SOFTWARE\\Bricsys\\Bricscad\\V");
				String sMajor( majorVersion() );
				sRootKey += sMajor;
				break;
			}
			return sRootKey;
		}
	String GetTargetModulePath( LPCTSTR pszModifier = NULL ) const
		{
			String sVersion( majorVersion() );
			String sAppFilename = GetAppName();
			if( pszModifier )
			{
				sAppFilename += _T(".");
				sAppFilename += pszModifier;
			}
			sAppFilename += _T(".");
			sAppFilename += sVersion;
			switch( platform() )
			{
			case kAutoCAD:
				sAppFilename += _T(".arx");
				break;
			case kBricscad:
				sAppFilename += _T(".brx");
				break;
			}
			return GetSiblingModulePath( sAppFilename );
		}
	void CreateDemandLoadEntry( RegKey& rkDemandLoad, LPCTSTR pszModifier = NULL ) const
		{
			if( !rkDemandLoad )
				return;
			if( platform() == kBricscad ||
					(platform() == kAutoCAD && majorVersion() >= kARX2004) )
			{
				rkDemandLoad.SetValue( REGKEY_LOADER, GetTargetModulePath( pszModifier ) );
				rkDemandLoad.SetValue( REGKEY_DESCRIPTION, GetAppLongName() );
				if( !rkDemandLoad.HasValue( REGVAL_LOADCTRLS ) )
					rkDemandLoad.SetValue( REGVAL_LOADCTRLS, kOnCommandInvocation | kOnLoadRequest/* | kOnAutoCADStartup*/ );
				{
					RegKey rkCommands( REGKEY_COMMANDS, rkDemandLoad, true, KEY_WRITE );
					LPCTSTR* rpszCommands = NULL;
					int idx = GetRxCommandArray( rpszCommands );
					while( idx-- > 0 )
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
					rkLoader.SetValue( _MODULE, GetTargetModulePath( pszModifier ) );
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
};


void RxSelfInstallImp( const TargetModule& Target, LPCTSTR pszTargetKey, bool bWantHKLM = true, bool bX64 = false )
{
	String sTargetKey = pszTargetKey;
	RegKey rkDemandLoad( sTargetKey + _T("\\Applications\\") + GetAppLongName(),
											 Target.GetTargetRootRegKey( bWantHKLM ),
											 true,
											 KEY_WRITE | (bX64? KEY_WOW64_64KEY : 0) );
	if( !rkDemandLoad )
		return;
	Target.CreateDemandLoadEntry( rkDemandLoad, (bX64? _T("x64") : NULL) );
}


bool EnumerateRegTargets( const TargetModule& Target, bool bWantHKLM = true, bool bX64 = false )
{
	String sRootKey = Target.GetTargetAppRegKey();
	HKEY hkTargetRoot = Target.GetTargetRootRegKey( bWantHKLM );
	RegKey rkTarget( sRootKey, hkTargetRoot, false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) );
	if( !rkTarget )
		return false;

	DWORD cchMaxSubkey = MAX_PATH;
	RegQueryInfoKey( rkTarget, NULL, NULL, NULL, NULL, &cchMaxSubkey, NULL, NULL, NULL, NULL, NULL, NULL );
	DWORD dwBufSize = ++cchMaxSubkey;
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
		dwBufSize = cchMaxSubkey;
		sSubkey.ReleaseBuffer();
		String sTargetKey( (LPCTSTR)sRootKey );
		sTargetKey += _T('\\');
		sTargetKey += sSubkey;
		if( bWantHKLM &&
				Target.platform() == TargetModule::kBricscad &&
				Target.majorVersion() == TargetModule::kBRX9 )
		{
			RegKey rkRoot( sTargetKey, HKEY_LOCAL_MACHINE, false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) );
			LPCTSTR pszLanguage = rkRoot.GetString( _T("Language") );
			if( pszLanguage && *pszLanguage )
			{
				sTargetKey = (LPCTSTR)sRootKey;
				sTargetKey += _T('\\');
				sTargetKey += pszLanguage;
				if( RegKey( sTargetKey, hkTargetRoot, false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) ) )
					continue; //already exists, so no need to create it
				RegKey rkLang( sTargetKey, hkTargetRoot, true, KEY_WRITE | (bX64? KEY_WOW64_64KEY : 0) );
				RegKey rkApps( _T("Applications"), rkLang, true, KEY_WRITE | (bX64? KEY_WOW64_64KEY : 0) );
			}
		}
		RxSelfInstallImp( Target, sTargetKey, bWantHKLM, bX64 );
	}
	return true;
}

extern "C"
__declspec(dllexport)
UINT __stdcall RxInstall( MSIHANDLE hInstall )
{
	String sInstallDir;
#ifdef _USEMSILIB
	String sCustomData;
	DWORD cchCustomData = 63;
	MsiGetProperty( hInstall, _T("CustomActionData"), sCustomData.GetBuffer( cchCustomData ), &cchCustomData );
	sCustomData.ReleaseBuffer( cchCustomData );
	if( cchCustomData <= 0 )
		return ERROR_INVALID_DATA;
	bool bWantHKLM = false;
	int nToken = sCustomData.Find( _T('>') );
	if( nToken >= 0 )
	{
		bWantHKLM = (sCustomData[nToken + 1] == _T('1'));
		sCustomData = sCustomData.Left( nToken );
	}
	sInstallDir = sCustomData;
#else
	bool bWantHKLM = true;
#endif
	//EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD14, sInstallDir ), bWantHKLM );
	//EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2000, sInstallDir ), bWantHKLM );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2004, sInstallDir ), bWantHKLM );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2005, sInstallDir ), bWantHKLM );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2006, sInstallDir ), bWantHKLM );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2007, sInstallDir ), bWantHKLM );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2008, sInstallDir ), bWantHKLM );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2009, sInstallDir ), bWantHKLM );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2010, sInstallDir ), bWantHKLM );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2011, sInstallDir ), bWantHKLM );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad9_3, sInstallDir ), bWantHKLM );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad10, sInstallDir ), bWantHKLM );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad11, sInstallDir ), bWantHKLM );
	if( IsWow64() )
	{
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2008, sInstallDir ), bWantHKLM, true );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2009, sInstallDir ), bWantHKLM, true );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2010, sInstallDir ), bWantHKLM, true );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2011, sInstallDir ), bWantHKLM, true );
	}
	return ERROR_SUCCESS;
}


bool RemoveAllRegTargets( LPCTSTR pszRegKey, HKEY hkRoot, bool bX64 = false )
{
	RegKey rkSoftware( _T("SOFTWARE"), hkRoot, false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) );
	RegKey rkTarget( pszRegKey, rkSoftware, false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) );
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
	//RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R14.0"), HKEY_LOCAL_MACHINE );
	//RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R15.0"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R16.0"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R16.0"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R16.1"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R16.1"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R16.2"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R16.2"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.0"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.0"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.1"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.1"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.2"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.2"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.0"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.0"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.1"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.1"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V9"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V9"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V10"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V10"), HKEY_CURRENT_USER );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V11"), HKEY_LOCAL_MACHINE );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V11"), HKEY_CURRENT_USER );
	if( IsWow64() )
	{
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.1"), HKEY_LOCAL_MACHINE, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.1"), HKEY_CURRENT_USER, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.2"), HKEY_LOCAL_MACHINE, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.2"), HKEY_CURRENT_USER, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.0"), HKEY_LOCAL_MACHINE, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.0"), HKEY_CURRENT_USER, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.1"), HKEY_LOCAL_MACHINE, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.1"), HKEY_CURRENT_USER, true );
	}
	return ERROR_SUCCESS;
}
