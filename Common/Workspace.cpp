// Workspace.cpp : implementation file
//

#include "stdafx.h"
#include "Workspace.h"

#pragma comment(lib, "version.lib") //for file version info functions

static const TCHAR gszRegVal_AUC[] = _T("AutoUpdateCheck");


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


CWorkspace::CWorkspace()
: mbMessagesSuppressed( false )
, mhmodLocalRes( NULL )
{
}

CWorkspace::~CWorkspace()
{
	FreeLibrary( mhmodLocalRes );
}

HMODULE CWorkspace::GetLocalResourceModule(void) const
{
	if( !mhmodLocalRes )
	{
		CString sResRoot;
		int cchPath = ::GetModuleFileName( GetResourceModule(), sResRoot.GetBuffer( MAX_PATH ), MAX_PATH );
		sResRoot.ReleaseBuffer( cchPath );
		if( sResRoot.IsEmpty() )
			return NULL;
		sResRoot.MakeReverse();
		CString sResModuleFilename = GetLocalResourceModuleFilename();
		if( sResModuleFilename.IsEmpty() )
		{
			CString sExt = sResRoot.SpanExcluding( _T(".") );
			CString sFilenameBase = sResRoot.Mid( sExt.GetLength() ).SpanExcluding( _T("\\/:") );
			sFilenameBase.MakeReverse();
			sExt.MakeReverse();
			sResModuleFilename = sFilenameBase + _T("Res.") + sExt;
		}
		sResRoot = sResRoot.Mid( sResRoot.SpanExcluding( _T("\\/:") ).GetLength() );
		sResRoot.MakeReverse();
		CString sLanguage = GetLanguage();
		CString sLanguageDir = sLanguage;
		if( !sLanguage.IsEmpty() )
			sLanguageDir += _T('\\');
	#ifdef _DEBUG
		//when running under the debugger, use the build project's language resource module (and default to ENU)
		if( sLanguage.IsEmpty() )
			sLanguage = _T("ENU");
	#ifdef EDITOR
		CString sResRootDbg = sResRoot;
		int cchBuildFolder = sResRootDbg.MakeReverse().Mid( 1 ).SpanExcluding( _T("\\/:") ).GetLength();
		sResRootDbg = sResRootDbg.Mid( cchBuildFolder + 1 ).MakeReverse();
		cchBuildFolder = sResRootDbg.MakeReverse().Mid( 1 ).SpanExcluding( _T("\\/:") ).GetLength();
		sResRootDbg = sResRootDbg.Mid( cchBuildFolder + 1 ).MakeReverse();
		CString sLanguageDirDbg;
		sLanguageDirDbg.Format( _T("%s\\Studio.Res\\Debug\\"), (LPCTSTR)sLanguage );
		CString sLocalResPathDbg = sResRootDbg + sLanguageDirDbg + sResModuleFilename;
		HMODULE hmodLocalRes = LoadLibrary( sLocalResPathDbg );
	#else
		CString sResRootDbg = sResRoot;
		int cchBuildFolder = sResRootDbg.MakeReverse().Mid( 1 ).SpanExcluding( _T("\\/:") ).GetLength();
		sResRootDbg = sResRootDbg.Mid( cchBuildFolder + 1 );
		cchBuildFolder = sResRootDbg.Mid( 1 ).SpanExcluding( _T("\\/:") ).GetLength();
		sResRootDbg = sResRootDbg.Mid( cchBuildFolder + 1 ).MakeReverse();
		CString sLanguageDirDbg;
		sLanguageDirDbg.Format( _T("%s\\Runtime.Res\\Debug\\"), (LPCTSTR)sLanguage );
		CString sLocalResPathDbg = sResRootDbg + sLanguageDirDbg + sResModuleFilename;
		HMODULE hmodLocalRes = LoadLibrary( sLocalResPathDbg );
	#endif
		if( !hmodLocalRes )
		{
			CString sLocalResPath = sResRoot + sLanguageDir + sResModuleFilename;
			hmodLocalRes = LoadLibrary( sLocalResPath );
		}
	#else
		CString sLocalResPath = sResRoot + sLanguageDir + sResModuleFilename;
		HMODULE hmodLocalRes = LoadLibrary( sLocalResPath );
	#endif
		if( !hmodLocalRes )
		{
			hmodLocalRes = LoadLibrary( sResRoot + sResModuleFilename );
			if( !hmodLocalRes )
			{
				hmodLocalRes = LoadLibrary( sResRoot + _T("ENU\\") + sResModuleFilename );
				if( hmodLocalRes && !sLanguage.IsEmpty() )
				{
					CString sMsg;
					sMsg.Format( _T("Missing language: %s\r\nReverting to US English"), (LPCTSTR)sLanguage );
					AfxMessageBox( sMsg, MB_OK | MB_ICONEXCLAMATION );
				}
			}
		}
		if( hmodLocalRes )
			const_cast< CWorkspace* >( this )->mhmodLocalRes = hmodLocalRes;
	}
	return mhmodLocalRes;
}

CString CWorkspace::LoadResourceString(int nResId, HMODULE hmodRes /*= NULL*/) const
{
	CString sRes;
	if( !hmodRes )
	{
		//CStringW sResW;
		//sResW.LoadString( GetLocalResourceModule(), nResId );
		//CStringA sResA;
		//int cchResA = WideCharToMultiByte( 936, 0, sResW, sResW.GetLength(), sResA.GetBuffer( 2000 ), 2000, NULL, NULL );
		//sResA.ReleaseBuffer( cchResA );
		sRes.LoadString( GetLocalResourceModule(), nResId );
		if( sRes.IsEmpty() )
			sRes.LoadString( GetResourceModule(), nResId );
	}
	else
		sRes.LoadString( hmodRes, nResId );
	return sRes;
}

CString CWorkspace::GetLanguage(void) const
{
	static const LPCTSTR pszRKLanguage = _T("Language");
	CString sLanguage;
	HKEY hkReg = NULL;
	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, GetSettingsRegPath(), 0, KEY_QUERY_VALUE, &hkReg ) )
	{
		DWORD dwType = 0;
		DWORD cbValue = 64 - sizeof(TCHAR);
		if( ERROR_SUCCESS == RegQueryValueEx( hkReg, pszRKLanguage, NULL, &dwType, (BYTE*)sLanguage.GetBuffer( 64 ), &cbValue ) &&
				dwType == REG_SZ )
			sLanguage.ReleaseBuffer();
		else
			sLanguage.ReleaseBuffer( 0 );
		RegCloseKey( hkReg );
	}
	if( !sLanguage.IsEmpty() )
		return sLanguage;
	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, GetSettingsRegPath(), 0, KEY_QUERY_VALUE, &hkReg ) )
	{
		DWORD dwType = 0;
		DWORD cbValue = 64 - sizeof(TCHAR);
		if( ERROR_SUCCESS == RegQueryValueEx( hkReg, pszRKLanguage, NULL, &dwType, (BYTE*)sLanguage.GetBuffer( 64 ), &cbValue ) &&
				dwType == REG_SZ )
			sLanguage.ReleaseBuffer();
		else
			sLanguage.ReleaseBuffer( 0 );
		RegCloseKey( hkReg );
	}
#ifdef _WIN64
	if( !sLanguage.IsEmpty() )
		return sLanguage;
	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, GetSettingsRegPath(), 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &hkReg ) )
	{
		DWORD dwType = 0;
		DWORD cbValue = 64 - sizeof(TCHAR);
		if( ERROR_SUCCESS == RegQueryValueEx( hkReg, pszRKLanguage, NULL, &dwType, (BYTE*)sLanguage.GetBuffer( 64 ), &cbValue ) &&
				dwType == REG_SZ )
			sLanguage.ReleaseBuffer();
		else
			sLanguage.ReleaseBuffer( 0 );
		RegCloseKey( hkReg );
	}
#endif //_WIN64
	return sLanguage;
	//if( !sLanguage.IsEmpty() )
	//	return sLanguage;
	//if( 0 != GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, sLanguage.GetBuffer( 64 ), 64 ) )
	//{
	//	sLanguage.ReleaseBuffer();
	//	return sLanguage;
	//}
	//return _T("ENU");
}

CString CWorkspace::GetLanguageSubfolderPath(void) const
{
	CString sLangSubfolder;
	int cchPath = ::GetModuleFileName( GetResourceModule(), sLangSubfolder.GetBuffer( MAX_PATH ), MAX_PATH );
	sLangSubfolder.ReleaseBuffer( cchPath );
	if( sLangSubfolder.IsEmpty() )
		return CString();
	sLangSubfolder.MakeReverse();
	sLangSubfolder = sLangSubfolder.Mid( sLangSubfolder.SpanExcluding( _T("\\/:") ).GetLength() );
	sLangSubfolder.MakeReverse();
	CString sLanguage = GetLanguage();
#ifdef _DEBUG
	//when running under the debugger, use the build project's language subfolder
	int cchBuildFolder = sLangSubfolder.MakeReverse().Mid( 1 ).SpanExcluding( _T("\\/:") ).GetLength();
	sLangSubfolder = sLangSubfolder.Mid( cchBuildFolder + 1 ).MakeReverse();
	cchBuildFolder = sLangSubfolder.MakeReverse().Mid( 1 ).SpanExcluding( _T("\\/:") ).GetLength();
	sLangSubfolder = sLangSubfolder.Mid( cchBuildFolder + 1 ).MakeReverse();
	sLanguage += _T("\\Content");
#endif
	sLangSubfolder = sLangSubfolder + sLanguage + _T("\\");
	return sLangSubfolder;
}

//display alert dialog; returns true if displayed, false if suppressed
bool CWorkspace::DisplayAlert( LPCTSTR pszMessage ) const
{
	assert(pszMessage != NULL);
	if( mbMessagesSuppressed )
		return false;
	AfxMessageBox( pszMessage );
	return true;
}

//display modeless status message; returns true if displayed, false if suppressed
bool CWorkspace::DisplayStatus( LPCTSTR pszMessage ) const
{
	return false; //need to use a status bar to display modeless messages in the editor [ORW]
}

bool CWorkspace::DisplayAlert( UINT nResourceId, HMODULE hmodRes /*= NULL*/ ) const
{
	return DisplayAlert( LoadResourceString( nResourceId, hmodRes ) );
}

bool CWorkspace::DisplayStatus( UINT nResourceId, HMODULE hmodRes /*= NULL*/ ) const
{
	return DisplayStatus( LoadResourceString( nResourceId, hmodRes ) );
}

void CWorkspace::SetModified( bool bModified /*= true*/ )
{
	CDocument* pDoc = GetActiveDocument();
	if( pDoc )
		pDoc->SetModifiedFlag( bModified? TRUE : FALSE );
}

bool CWorkspace::GetDwordSetting( DWORD& dwValue, LPCTSTR pszValueName ) const
{
	if( GetDwordSettingImp( dwValue, pszValueName, false ) )
		return true;
	if( GetDwordSettingImp( dwValue, pszValueName, true ) )
		return true;
#ifdef _WIN64
	//check the 64 bit registry view in case the setting is stored there
	if( GetDwordSettingImp( dwValue, pszValueName, false, false ) )
		return true;
	if( GetDwordSettingImp( dwValue, pszValueName, true, false ) )
		return true;
#endif //_WIN64
	return false;
}

bool CWorkspace::SetDwordSetting( DWORD dwValue, LPCTSTR pszValueName )
{ //always writing to 32-bit registry view
	bool bSuccess = false;
	HKEY hkReg = NULL;
	if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, GetSettingsRegPath(), 0, NULL, 0, KEY_SET_VALUE | KEY_WOW64_32KEY, NULL, &hkReg, NULL ) )
	{
		if( ERROR_SUCCESS == RegSetValueEx( hkReg, pszValueName, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(dwValue) ) )
			bSuccess = true;
		RegCloseKey( hkReg );
	}
	return bSuccess;
}

bool CWorkspace::GetModuleVersionInfo( DWORD& dwMajor, DWORD& dwMinor, DWORD& dwThird, DWORD& dwFourth,
																			 HMODULE hmodTarget /*= NULL*/ ) const
{
	if( !hmodTarget )
		hmodTarget = GetModuleHandle( NULL );
	CHAR szaModulePath[MAX_PATH];
	if( GetModuleFileNameA( hmodTarget, szaModulePath, MAX_PATH ) <= 0 )
		return false;
	DWORD dwHandle;
	DWORD dwVerSize = GetFileVersionInfoSizeA( szaModulePath, &dwHandle );
	if( dwVerSize == 0 )
		return false;

	PVOID pvBuffer = new BYTE[dwVerSize];
	if( GetFileVersionInfoA( szaModulePath, dwHandle, dwVerSize, pvBuffer ) )
	{
		VS_FIXEDFILEINFO*	pFixedInfo;
		UINT nSize;
		if( VerQueryValueA( pvBuffer, "\\", reinterpret_cast< void** >(&pFixedInfo), &nSize ) )
		{
			DWORD dwVersionMS = pFixedInfo->dwFileVersionMS;
			DWORD dwVersionLS = pFixedInfo->dwFileVersionLS;
			dwMajor = HIWORD(dwVersionMS);
			dwMinor = LOWORD(dwVersionMS);
			dwThird = HIWORD(dwVersionLS);
			dwFourth = LOWORD(dwVersionLS);
		}
	}
	delete[] pvBuffer;
	return true;
}

bool CWorkspace::IsAutoUpdateCheckEnabled(void) const
{
	DWORD dwAUC = 0;
	return (GetDwordSetting( dwAUC, gszRegVal_AUC ) && dwAUC != 0);
}

bool CWorkspace::SetAutoUpdateCheckEnabled( bool bEnabled /*= true*/ )
{
	return SetDwordSetting( bEnabled? 1 : 0, gszRegVal_AUC );
}

CString CWorkspace::FindFile( LPCTSTR pszFilePath ) const
{
	CString sPath;
	DWORD ctPath = SearchPath( NULL, pszFilePath, NULL, MAX_PATH, sPath.GetBuffer( MAX_PATH ), NULL );
	sPath.ReleaseBuffer();
	if( ctPath == 0 )
	{ //check the language and app folder yet
		CString sLanguageFolder = theWorkspace.GetLanguageSubfolderPath();
		ctPath = SearchPath( sLanguageFolder, pszFilePath, NULL, MAX_PATH, sPath.GetBuffer( MAX_PATH ), NULL );
		sPath.ReleaseBuffer();
		if( sPath.IsEmpty() )
		{
			CString sAppFolder;
			if( 0 < GetModuleFileName( NULL, sAppFolder.GetBuffer( MAX_PATH ), MAX_PATH ) )
			{
				sAppFolder.ReleaseBuffer();
				sAppFolder.MakeReverse();
				sAppFolder = sAppFolder.Mid( sAppFolder.SpanExcluding( _T("\\/:") ).GetLength() );
				sAppFolder.MakeReverse();
				ctPath = SearchPath( sAppFolder, pszFilePath, NULL, MAX_PATH, sPath.GetBuffer( MAX_PATH ), NULL );
				sPath.ReleaseBuffer();
			}
		}
	}
	return sPath;
}

bool CWorkspace::GetDwordSettingImp( DWORD& dwValue, LPCTSTR pszValueName, bool bHKLM /*= true*/, bool bX32 /*= true*/ ) const
{
	bool bSuccess = false;
	DWORD dwValueLocal = 0;
	HKEY hkReg = NULL;
	if( ERROR_SUCCESS == RegOpenKeyEx( bHKLM? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER,
																		 GetSettingsRegPath(),
																		 0,
																		 KEY_QUERY_VALUE | (bX32? KEY_WOW64_32KEY : 0),
																		 &hkReg ) )
	{
		DWORD dwType = 0;
		DWORD cbValue = sizeof(dwValueLocal);
		if( ERROR_SUCCESS == RegQueryValueEx( hkReg, pszValueName, NULL, &dwType, (BYTE*)&dwValueLocal, &cbValue ) )
			bSuccess = true;
		RegCloseKey( hkReg );
	}
	if( !bSuccess )
		return false;
	dwValue = dwValueLocal;
	return true;
}
