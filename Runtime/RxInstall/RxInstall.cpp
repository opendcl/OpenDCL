// RxInstall.cpp : Install-time AutoCAD ObjectARX module demand-load setup
//
// Copyright 2007 - 2020 ManuSoft. All Rights Reserved.
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

static UINT GetRxRuntimeCommandArray( LPCTSTR*& rpszCommands )
{
	static LPCTSTR rszCommands[] =
	{
		_T("OPENDCL"),
	};
	rpszCommands = rszCommands;
	return _elements(rszCommands);
}

static UINT GetRxStudioCommandArray( LPCTSTR*& rpszCommands )
{
	static LPCTSTR rszCommands[] =
	{
		_T("OPENDCL"),
		_T("OPENDCLDEMO"),
	};
	rpszCommands = rszCommands;
	return _elements(rszCommands);
}

static bool IsStudioInstalled()
{
	static bool bIsStudioInstalled = (NULL != RegKey( _T("OpenDCL.Project"), HKEY_CLASSES_ROOT, false, KEY_READ ));
	return bIsStudioInstalled;
}

static UINT GetRxCommandArray( LPCTSTR*& rpszCommands )
{
	return (IsStudioInstalled()? GetRxStudioCommandArray( rpszCommands ) : GetRxRuntimeCommandArray( rpszCommands ));
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
	enum Architecture { kAny = 0, kX86, kX64, };
	enum Platform { kNone = 0, kAutoCAD, kBricscad, kZWCAD, kGstarCAD };
	enum MajorVersion { kBRX9 = 9, kBRX10 = 10, kBRX11 = 11, kBRX12 = 12,
											kBRX13 = 13, kBRX14 = 14, kBRX15 = 15, kBRX16 = 16, kBRX17 = 17,
											kBRX18 = 18, kBRX19 = 19, kBRX20 = 20, kBRX21 = 21,
											kBRX22 = 22,
											kZRX2014 = 14, kZRX2015 = 15, kZRX2017 = 17, kZRX2018 = 18,
											kZRX2019 = 19, kZRX2020 = 20, kZRX2021 = 21, kZRX2022 = 22,
											kGRX2015 = 15, kGRX2016 = 16, kGRX2017 = 17, kGRX2018 = 18,
											kGRX2019 = 19, kGRX2020 = 20, kGRX2021 = 21,
											kARX1 = 13, kARX2 = 14, kARX2000 = 15, kARX2004 = 16,
											kARX2007 = 17, kARX2010 = 18, kARX2013 = 19, kARX2015 = 20,
											kARX2017 = 21, kARX2018 = 22, kARX2019 = 23, kARX2021 = 24, };
	typedef char TByte;
	TargetModule( Architecture architecture, Platform platform, MajorVersion majorVer, TByte minorVer, LPCTSTR pszInstallDir = NULL )
		: mArchitecture( architecture )
		, mPlatform( platform )
		, mMajorVer( majorVer )
		, mMinorVer( minorVer )
		, msInstallDir( pszInstallDir )
		{}
	TargetModule( unsigned int target, LPCTSTR pszInstallDir = NULL )
		: mArchitecture( static_cast< Platform >((TByte)(target >> shftArchitecture)) )
		, mPlatform( static_cast< Platform >((TByte)(target >> shftPlatform)) )
		, mMajorVer( static_cast< MajorVersion >((TByte)(target >> shftMajorVer)) )
		, mMinorVer( (TByte)target )
		, msInstallDir( pszInstallDir )
		{}
	TargetModule( const TargetModule& Src )
		: mArchitecture( Src.mArchitecture )
		, mPlatform( Src.mPlatform )
		, mMajorVer( Src.mMajorVer )
		, mMinorVer( Src.mMinorVer )
		, msInstallDir( (LPCTSTR)Src.msInstallDir )
		{}
	operator unsigned int() const { return ((mArchitecture << sizeof(shftArchitecture)) | (mPlatform << sizeof(shftPlatform)) | (mMajorVer << sizeof(shftMajorVer)) | mMinorVer); }
	Architecture architecture() const { return static_cast< Architecture >(mArchitecture); }
	Platform platform() const { return static_cast< Platform >(mPlatform); }
	MajorVersion majorVersion() const { return static_cast< MajorVersion >(mMajorVer); }
	TByte minorVersion() const { return mMinorVer; }
	LPCTSTR installDir() const { return msInstallDir; }
protected:
	TByte mArchitecture;
	TByte mPlatform;
	TByte mMajorVer;
	TByte mMinorVer;
	String msInstallDir;
	static const int shftArchitecture = ((sizeof(TByte) + sizeof(TByte) + sizeof(TByte)) << 3);
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
		kX86Architecture = (kX86 << shftArchitecture),
		kX64Architecture = (kX64 << shftArchitecture),
		kAutoCADPlatform = (kAutoCAD << shftPlatform),
		kBricscadPlatform = (kBricscad << shftPlatform),
		kZWCADPlatform = (kZWCAD << shftPlatform),
		kGstarCADPlatform = (kGstarCAD << shftPlatform),

		kAutoCAD14 =         (kX86Architecture | kAutoCADPlatform | (kARX2 << shftMajorVer) | 0),
		kAutoCAD2000 =       (kX86Architecture | kAutoCADPlatform | (kARX2000 << shftMajorVer) | 0),
		kAutoCAD2000i =      (kX86Architecture | kAutoCADPlatform | (kARX2000 << shftMajorVer) | 0),
		kAutoCAD2002 =       (kX86Architecture | kAutoCADPlatform | (kARX2000 << shftMajorVer) | 0),
		kAutoCAD2004 =       (kX86Architecture | kAutoCADPlatform | (kARX2004 << shftMajorVer) | 0),
		kAutoCAD2005 =       (kX86Architecture | kAutoCADPlatform | (kARX2004 << shftMajorVer) | 1),
		kAutoCAD2006 =       (kX86Architecture | kAutoCADPlatform | (kARX2004 << shftMajorVer) | 2),
		kAutoCAD2007 =       (kX86Architecture | kAutoCADPlatform | (kARX2007 << shftMajorVer) | 0),
		kAutoCAD2008x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2007 << shftMajorVer) | 1),
		kAutoCAD2008x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2007 << shftMajorVer) | 1),
		kAutoCAD2009x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2007 << shftMajorVer) | 2),
		kAutoCAD2009x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2007 << shftMajorVer) | 2),
		kAutoCAD2010x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2010 << shftMajorVer) | 0),
		kAutoCAD2010x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2010 << shftMajorVer) | 0),
		kAutoCAD2011x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2010 << shftMajorVer) | 1),
		kAutoCAD2011x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2010 << shftMajorVer) | 1),
		kAutoCAD2012x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2010 << shftMajorVer) | 2),
		kAutoCAD2012x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2010 << shftMajorVer) | 2),
		kAutoCAD2013x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2013 << shftMajorVer) | 0),
		kAutoCAD2013x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2013 << shftMajorVer) | 0),
		kAutoCAD2014x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2013 << shftMajorVer) | 1),
		kAutoCAD2014x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2013 << shftMajorVer) | 1),
		kAutoCAD2015x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2015 << shftMajorVer) | 0),
		kAutoCAD2015x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2015 << shftMajorVer) | 0),
		kAutoCAD2016x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2015 << shftMajorVer) | 1),
		kAutoCAD2016x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2015 << shftMajorVer) | 1),
		kAutoCAD2017x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2017 << shftMajorVer) | 0),
		kAutoCAD2017x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2017 << shftMajorVer) | 0),
		kAutoCAD2018x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2018 << shftMajorVer) | 0),
		kAutoCAD2018x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2018 << shftMajorVer) | 0),
		kAutoCAD2019x86 =    (kX86Architecture | kAutoCADPlatform | (kARX2019 << shftMajorVer) | 0),
		kAutoCAD2019x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2019 << shftMajorVer) | 0),
		kAutoCAD2020x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2019 << shftMajorVer) | 1),
		kAutoCAD2021x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2021 << shftMajorVer) | 0),
		kAutoCAD2022x64 =    (kX64Architecture | kAutoCADPlatform | (kARX2021 << shftMajorVer) | 1),
		kBricscad9_3 =       (kX86Architecture | kBricscadPlatform | (kBRX9 << shftMajorVer) | 3),
		kBricscad10 =        (kX86Architecture | kBricscadPlatform | (kBRX10 << shftMajorVer) | 0),
		kBricscad11 =        (kX86Architecture | kBricscadPlatform | (kBRX11 << shftMajorVer) | 0),
		kBricscad12 =        (kX86Architecture | kBricscadPlatform | (kBRX12 << shftMajorVer) | 0),
		kBricscad13x86 =     (kX86Architecture | kBricscadPlatform | (kBRX13 << shftMajorVer) | 0),
		kBricscad13x64 =     (kX64Architecture | kBricscadPlatform | (kBRX13 << shftMajorVer) | 0),
		kBricscad14x86 =     (kX86Architecture | kBricscadPlatform | (kBRX14 << shftMajorVer) | 0),
		kBricscad14x64 =     (kX64Architecture | kBricscadPlatform | (kBRX14 << shftMajorVer) | 0),
		kBricscad15x86 =     (kX86Architecture | kBricscadPlatform | (kBRX15 << shftMajorVer) | 0),
		kBricscad15x64 =     (kX64Architecture | kBricscadPlatform | (kBRX15 << shftMajorVer) | 0),
		kBricscad16x86 =     (kX86Architecture | kBricscadPlatform | (kBRX16 << shftMajorVer) | 0),
		kBricscad16x64 =     (kX64Architecture | kBricscadPlatform | (kBRX16 << shftMajorVer) | 0),
		kBricscad17x86 =     (kX86Architecture | kBricscadPlatform | (kBRX17 << shftMajorVer) | 0),
		kBricscad17x64 =     (kX64Architecture | kBricscadPlatform | (kBRX17 << shftMajorVer) | 0),
		kBricscad18x86 =     (kX86Architecture | kBricscadPlatform | (kBRX18 << shftMajorVer) | 0),
		kBricscad18x64 =     (kX64Architecture | kBricscadPlatform | (kBRX18 << shftMajorVer) | 0),
		kBricscad19x86 =     (kX86Architecture | kBricscadPlatform | (kBRX19 << shftMajorVer) | 0),
		kBricscad19x64 =     (kX64Architecture | kBricscadPlatform | (kBRX19 << shftMajorVer) | 0),
		kBricscad20x86 =     (kX86Architecture | kBricscadPlatform | (kBRX20 << shftMajorVer) | 0),
		kBricscad20x64 =     (kX64Architecture | kBricscadPlatform | (kBRX20 << shftMajorVer) | 0),
		kBricscad21x64 =     (kX64Architecture | kBricscadPlatform | (kBRX21 << shftMajorVer) | 0),
		kBricscad22x64 =     (kX64Architecture | kBricscadPlatform | (kBRX22 << shftMajorVer) | 0),
		kZWCAD2014x86 =      (kX86Architecture | kZWCADPlatform | (kZRX2014 << shftMajorVer) | 0),
		kZWCAD2015x86 =      (kX86Architecture | kZWCADPlatform | (kZRX2015 << shftMajorVer) | 0),
		kZWCAD2017x86 =      (kX86Architecture | kZWCADPlatform | (kZRX2017 << shftMajorVer) | 0),
		kZWCAD2017x64 =      (kX64Architecture | kZWCADPlatform | (kZRX2017 << shftMajorVer) | 0),
		kZWCAD2018x86 =      (kX86Architecture | kZWCADPlatform | (kZRX2018 << shftMajorVer) | 0),
		kZWCAD2018x64 =      (kX64Architecture | kZWCADPlatform | (kZRX2018 << shftMajorVer) | 0),
		kZWCAD2019x86 =      (kX86Architecture | kZWCADPlatform | (kZRX2019 << shftMajorVer) | 0),
		kZWCAD2019x64 =      (kX64Architecture | kZWCADPlatform | (kZRX2019 << shftMajorVer) | 0),
		kZWCAD2020x86 =      (kX86Architecture | kZWCADPlatform | (kZRX2020 << shftMajorVer) | 0),
		kZWCAD2020x64 =      (kX64Architecture | kZWCADPlatform | (kZRX2020 << shftMajorVer) | 0),
		kZWCAD2021x86 =      (kX86Architecture | kZWCADPlatform | (kZRX2021 << shftMajorVer) | 0),
		kZWCAD2021x64 =      (kX64Architecture | kZWCADPlatform | (kZRX2021 << shftMajorVer) | 0),
		kZWCAD2022x86 =      (kX86Architecture | kZWCADPlatform | (kZRX2022 << shftMajorVer) | 0),
		kZWCAD2022x64 =      (kX64Architecture | kZWCADPlatform | (kZRX2022 << shftMajorVer) | 0),
		kGstarCAD2015x86 =      (kX86Architecture | kGstarCADPlatform | (kGRX2015 << shftMajorVer) | 0),
		kGstarCAD2015x64 =      (kX64Architecture | kGstarCADPlatform | (kGRX2015 << shftMajorVer) | 0),
		kGstarCAD2016x86 =      (kX86Architecture | kGstarCADPlatform | (kGRX2016 << shftMajorVer) | 0),
		kGstarCAD2016x64 =      (kX64Architecture | kGstarCADPlatform | (kGRX2016 << shftMajorVer) | 0),
		kGstarCAD2017x86 =      (kX86Architecture | kGstarCADPlatform | (kGRX2017 << shftMajorVer) | 0),
		kGstarCAD2017x64 =      (kX64Architecture | kGstarCADPlatform | (kGRX2017 << shftMajorVer) | 0),
		kGstarCAD2018x86 =      (kX86Architecture | kGstarCADPlatform | (kGRX2018 << shftMajorVer) | 0),
		kGstarCAD2018x64 =      (kX64Architecture | kGstarCADPlatform | (kGRX2018 << shftMajorVer) | 0),
		kGstarCAD2019x86 =      (kX86Architecture | kGstarCADPlatform | (kGRX2019 << shftMajorVer) | 0),
		kGstarCAD2019x64 =      (kX64Architecture | kGstarCADPlatform | (kGRX2019 << shftMajorVer) | 0),
		kGstarCAD2020x86 =      (kX86Architecture | kGstarCADPlatform | (kGRX2020 << shftMajorVer) | 0),
		kGstarCAD2020x64 =      (kX64Architecture | kGstarCADPlatform | (kGRX2020 << shftMajorVer) | 0),
		kGstarCAD2021x86 =      (kX86Architecture | kGstarCADPlatform | (kGRX2021 << shftMajorVer) | 0),
		kGstarCAD2021x64 =      (kX64Architecture | kGstarCADPlatform | (kGRX2021 << shftMajorVer) | 0),
	};

	HKEY GetTargetRootRegKey( bool bWantHKLM = true ) const
		{
			if( platform() == kAutoCAD && majorVersion() < kARX2004 )
				return (isHKLMAccessible()? HKEY_LOCAL_MACHINE : NULL);
			if( bWantHKLM && isHKLMAccessible() )
				return HKEY_LOCAL_MACHINE;
			return HKEY_CURRENT_USER;
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
				{
					sRootKey = _T("SOFTWARE\\Bricsys\\Bricscad\\V");
					String sMajor( majorVersion() );
					sRootKey += sMajor;
					switch( architecture() )
					{
					case kX64:
						sRootKey += _T("x64");
						break;
					}
				}
				break;
			case kZWCAD:
				{
					sRootKey = _T("SOFTWARE\\ZWSOFT\\ZWCAD\\");
					String sMajor( 2000 + majorVersion() );
					sRootKey += sMajor;
				}
				break;
			case kGstarCAD:
				{
					sRootKey = _T("SOFTWARE\\GstarSoft\\GstarCAD\\R");
					String sMajor( majorVersion() );
					sRootKey += sMajor;
					if( majorVersion() < 18 || minorVersion() > 0 )
					{
						sRootKey += _T('.');
						String sMinor( minorVersion() );
						sRootKey += sMinor;
					}
				}
				break;
			}
			return sRootKey;
		}
	String GetTargetModulePath( LPCTSTR pszModifier = NULL ) const
		{
			String sAppFilename = GetAppName();
			if( pszModifier )
			{
				sAppFilename += _T(".");
				sAppFilename += pszModifier;
			}
			sAppFilename += _T(".");
			switch( platform() )
			{
			case kAutoCAD:
				{
					String sVersion( majorVersion() );
					sAppFilename += sVersion;
					sAppFilename += _T(".arx");
				}
				break;
			case kBricscad:
				{
					String sVersion( majorVersion() );
					sAppFilename += sVersion;
					sAppFilename += _T(".brx");
				}
				break;
			case kZWCAD:
				{
					String sVersion( 2000 + majorVersion() );
					sAppFilename += sVersion;
					sAppFilename += _T(".zrx");
				}
				break;
			case kGstarCAD:
				{
					String sVersion( 2000 + majorVersion() );
					sAppFilename += sVersion;
					sAppFilename += _T(".grx");
				}
				break;
			}
			return GetSiblingModulePath( sAppFilename );
		}
	void CreateDemandLoadEntry( RegKey& rkDemandLoad, bool bLoadOnStartup = false, LPCTSTR pszModifier = NULL ) const
		{
			if( !rkDemandLoad )
				return;
			if( platform() == kBricscad ||
					platform() == kZWCAD ||
					platform() == kGstarCAD ||
					(platform() == kAutoCAD && majorVersion() >= kARX2004) )
			{
				rkDemandLoad.SetValue( REGKEY_LOADER, GetTargetModulePath( pszModifier ) );
				rkDemandLoad.SetValue( REGKEY_DESCRIPTION, GetAppLongName() );
				if( !rkDemandLoad.HasValue( REGVAL_LOADCTRLS ) )
					rkDemandLoad.SetValue( REGVAL_LOADCTRLS, kOnCommandInvocation | kOnLoadRequest | (bLoadOnStartup? kOnAutoCADStartup : 0) );
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
					rkDemandLoad.SetValue( REGVAL_LOADCTRLS, kOnCommandInvocation | kOnLoadRequest | (bLoadOnStartup? kOnAutoCADStartup : 0) );
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


void RxSelfInstallImp( const TargetModule& Target, LPCTSTR pszTargetKey, bool bWantHKLM = true, bool bLoadOnStartup = false )
{
	bool bX64 = (Target.architecture() == TargetModule::kX64);
	HKEY hkRoot = Target.GetTargetRootRegKey( bWantHKLM );
	String sTargetKey = pszTargetKey;
	sTargetKey += _T("\\Applications");
	if( !RegKey( sTargetKey, hkRoot, false, KEY_READ | (bX64? KEY_WOW64_64KEY : 0) ) )
	{
		//no 'Applications' subkey
		switch( Target.platform() )
		{
		case TargetModule::kAutoCAD:
			return; //skip AutoCAD keys with no existing 'Applications' subkey
		}
	}
	sTargetKey += _T('\\');
	sTargetKey += GetAppLongName();
	RegKey rkDemandLoad( sTargetKey,
											 hkRoot,
											 true,
											 KEY_WRITE | (bX64? KEY_WOW64_64KEY : 0) );
	if( !rkDemandLoad )
		return;
	Target.CreateDemandLoadEntry( rkDemandLoad, bLoadOnStartup, (bX64? _T("x64") : NULL) );
}


bool EnumerateRegTargets( const TargetModule& Target, bool bWantHKLM = true, bool bLoadOnStartup = false )
{
	bool bX64 = (Target.architecture() == TargetModule::kX64);
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
		RxSelfInstallImp( Target, sTargetKey, bWantHKLM, bLoadOnStartup );
	}
	return true;
}

void InstallAllTargets( LPCTSTR pszInstallDir, bool bWantHKLM, bool bLoadOnStartup )
{
	EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2015x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2016x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2017x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2018x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2019x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2020x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2021x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2014x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2015x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2017x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2018x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2019x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2020x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2021x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2022x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad9_3, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad10, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad11, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad12, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad13x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad14x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad15x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad16x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad17x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad18x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad19x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kBricscad20x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	//EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD14, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	//EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2000, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2004, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2005, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2006, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2007, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2008x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2009x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2010x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2011x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2012x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2013x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2014x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2015x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2016x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2017x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2018x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2019x86, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	if( IsWow64() )
	{
		EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2015x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2016x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2017x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2018x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2019x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2020x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kGstarCAD2021x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2017x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2018x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2019x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2020x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2021x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kZWCAD2022x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kBricscad13x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kBricscad14x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kBricscad15x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kBricscad16x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kBricscad17x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kBricscad18x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kBricscad19x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kBricscad20x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kBricscad21x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kBricscad22x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2008x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2009x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2010x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2011x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2012x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2013x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2014x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2015x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2016x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2017x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2018x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2019x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2020x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2021x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
		EnumerateRegTargets( TargetModule( TargetModule::kAutoCAD2022x64, pszInstallDir ), bWantHKLM, bLoadOnStartup );
	}
}

UINT RxInstall( MSIHANDLE hInstall, bool bWantHKLM )
{
	String sInstallDir;
#ifdef _USEMSILIB
	String sCustomData;
	DWORD cchCustomData = 63;
	MsiGetProperty( hInstall, _T("CustomActionData"), sCustomData.GetBuffer( cchCustomData ), &cchCustomData );
	sCustomData.ReleaseBuffer( cchCustomData );
	if( cchCustomData <= 0 )
		return ERROR_INVALID_DATA;
	sInstallDir = sCustomData;
#endif

	// Decide whether to autostart the OPENDCLDEMO command on next startup
	bool bLoadOnNextStartup = false;
	if( IsStudioInstalled() )
	{
		DWORD dwLastAutoStart = (DWORD)-1;
		RegKey rkCU( _T("Software\\OpenDCL"), HKEY_CURRENT_USER, false, KEY_READ | KEY_WRITE );
		if (rkCU)
		{
			dwLastAutoStart = rkCU.GetDword( _T("LastAutoStart") );
			if( dwLastAutoStart != (DWORD)-1 )
			{
				SYSTEMTIME st;
				GetSystemTime( &st );
				DWORD dwNow = (((st.wYear - 2000) * 365) + ((st.wMonth - 1) * 30) + st.wDay); //approximate number of days since 2000-01-01
				rkCU.SetValue( _T("LastAutoStart"), dwNow );
				bLoadOnNextStartup = ((dwLastAutoStart == 0) || (dwNow - dwLastAutoStart >= 365));
				rkCU.SetValue( _T("DisableAutoStartOnNextLoad"), bLoadOnNextStartup? 1 : 0 );
			}
		}
	}

	if (bWantHKLM && bLoadOnNextStartup)
	{
		InstallAllTargets( sInstallDir, true, false );
		InstallAllTargets( sInstallDir, false, true );
	}
	else
		InstallAllTargets( sInstallDir, bWantHKLM, bLoadOnNextStartup );
	return ERROR_SUCCESS;
}

extern "C"
__declspec(dllexport)
UINT __stdcall RxInstallMachine( MSIHANDLE hInstall )
{
	return RxInstall( hInstall, true );
}

extern "C"
__declspec(dllexport)
UINT __stdcall RxInstallUser( MSIHANDLE hInstall )
{
	return RxInstall( hInstall, false );
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

void UninstallAllTargets( HKEY hkRoot )
{
	//RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R14.0"), hkRoot );
	//RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R15.0"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R16.0"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R16.1"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R16.2"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.0"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.1"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.2"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.0"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.1"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.2"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R19.0"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R19.1"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R20.0"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R20.1"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R21.0"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R22.0"), hkRoot );
	RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R23.0"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V9"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V10"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V11"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V12"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V13"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V14"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V15"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V16"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V17"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V18"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V19"), hkRoot );
	RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V20"), hkRoot );
	RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2014"), hkRoot );
	RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2015"), hkRoot );
	RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2017"), hkRoot );
	RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2018"), hkRoot );
	RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2019"), hkRoot );
	RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2020"), hkRoot );
	RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2021"), hkRoot );
	RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2022"), hkRoot );
	RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R15.0"), hkRoot );
	RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R16.0"), hkRoot );
	RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R17.0"), hkRoot );
	RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R18"), hkRoot );
	RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R19"), hkRoot );
	RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R20"), hkRoot );
	RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R21"), hkRoot );
	if( IsWow64() )
	{
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.1"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R17.2"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.0"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.1"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R18.2"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R19.0"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R19.1"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R20.0"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R20.1"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R21.0"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R22.0"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R23.0"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R23.1"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R24.0"), hkRoot, true );
		RemoveAllRegTargets( _T("Autodesk\\AutoCAD\\R24.1"), hkRoot, true );
		RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V13x64"), hkRoot, true );
		RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V14x64"), hkRoot, true );
		RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V15x64"), hkRoot, true );
		RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V16x64"), hkRoot, true );
		RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V17x64"), hkRoot, true );
		RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V18x64"), hkRoot, true );
		RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V19x64"), hkRoot, true );
		RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V20x64"), hkRoot, true );
		RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V21x64"), hkRoot, true );
		RemoveAllRegTargets( _T("Bricsys\\Bricscad\\V22x64"), hkRoot, true );
		RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2017"), hkRoot, true );
		RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2018"), hkRoot, true );
		RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2019"), hkRoot, true );
		RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2020"), hkRoot, true );
		RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2021"), hkRoot, true );
		RemoveAllRegTargets( _T("ZWSOFT\\ZWCAD\\2022"), hkRoot, true );
		RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R15.0"), hkRoot, true );
		RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R16.0"), hkRoot, true );
		RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R17.0"), hkRoot, true );
		RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R18"), hkRoot, true );
		RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R19"), hkRoot, true );
		RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R20"), hkRoot, true );
		RemoveAllRegTargets( _T("Gstarsoft\\GstarCAD\\R21"), hkRoot, true );
	}
}

extern "C"
__declspec(dllexport)
UINT __stdcall RxUninstallMachine( MSIHANDLE hInstall )
{
	UninstallAllTargets( HKEY_LOCAL_MACHINE );
	return ERROR_SUCCESS;
}

extern "C"
__declspec(dllexport)
UINT __stdcall RxUninstallUser( MSIHANDLE hInstall )
{
	UninstallAllTargets( HKEY_CURRENT_USER );
	return ERROR_SUCCESS;
}
