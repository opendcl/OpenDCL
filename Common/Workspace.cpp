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
{
}

CWorkspace::~CWorkspace()
{
}

CString CWorkspace::LoadResourceString(int nResId, HMODULE hmodRes /*= NULL*/) const
{
	CString sRes;
	sRes.LoadString( hmodRes? hmodRes : GetLocalResourceModule(), nResId );
	return sRes;
}

CString CWorkspace::GetLanguage(void)
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
			sLanguage.ReleaseBuffer( cbValue / sizeof(TCHAR) );
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
			sLanguage.ReleaseBuffer( cbValue / sizeof(TCHAR) );
		else
			sLanguage.ReleaseBuffer( 0 );
		RegCloseKey( hkReg );
	}
	if( !sLanguage.IsEmpty() )
		return sLanguage;
	if( 0 != GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, sLanguage.GetBuffer( 64 ), 64 ) )
	{
		sLanguage.ReleaseBuffer();
		return sLanguage;
	}
	return _T("ENU");
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

bool CWorkspace::GetDwordSetting( DWORD& dwValue, LPCTSTR pszValueName )
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
	return (theWorkspace.GetDwordSetting( dwAUC, gszRegVal_AUC ) && dwAUC != 0);
}

bool CWorkspace::SetAutoUpdateCheckEnabled( bool bEnabled /*= true*/ )
{
	return theWorkspace.SetDwordSetting( bEnabled? 1 : 0, gszRegVal_AUC );
}

CString CWorkspace::FindFile( LPCTSTR pszFilePath ) const
{
	CString sPath;
	DWORD ctPath = SearchPath( NULL, pszFilePath, NULL, MAX_PATH, sPath.GetBuffer( MAX_PATH ), NULL );
	sPath.ReleaseBuffer();
	if( ctPath == 0 )
	{ //check the app folder yet
		CString sAppFolder;
		if( 0 < GetModuleFileName( NULL, sAppFolder.GetBuffer( MAX_PATH ), MAX_PATH ) )
		{
			sAppFolder.ReleaseBuffer();
			sAppFolder = sAppFolder.Mid( sAppFolder.MakeReverse().SpanExcluding( _T("\\/:") ).GetLength() );
			sAppFolder.MakeReverse();
			DWORD ctPath = SearchPath( sAppFolder, pszFilePath, NULL, MAX_PATH, sPath.GetBuffer( MAX_PATH ), NULL );
			sPath.ReleaseBuffer();
		}
	}
	return sPath;
}

bool CWorkspace::GetDwordSettingImp( DWORD& dwValue, LPCTSTR pszValueName, bool bHKLM /*= true*/, bool bX32 /*= true*/ )
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
