// Workspace.cpp : implementation file
//

#include "stdafx.h"
#include "Workspace.h"

#pragma comment(lib, "version.lib") //for file version info functions


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
	if( hmodRes )
		sRes.LoadString( hmodRes, nResId );
	else
		sRes.LoadString( nResId );
	return sRes;
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

bool CWorkspace::GetModuleVersionInfo( DWORD& dwMajor, DWORD&dwMinor, DWORD& dwThird, DWORD& dwFourth,
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
