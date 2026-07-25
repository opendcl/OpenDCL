// UpdateCheck.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateCheck.h"
#include "Workspace.h"
#include "Resource.h"
#include <memory>
#include <set>
#include <WinINet.h>
#include <ShellAPI.h>

#pragma comment( lib, "Wininet.lib" )

#ifndef NIN_BALLOONUSERCLICK
#define NIN_BALLOONUSERCLICK    (WM_USER + 5) //from ShellAPI.h
#endif

static const UINT WM_MMTRAY_NOTIFY = WM_USER + 50;
static const UINT ID_TOGGLEUPDATECHECK = WM_USER + 51;

// Plain-text version endpoints on the static site (opendcl.github.io / opendcl.com).
static const TCHAR gszVersionHostUrlStable[] =
	_T("https://www.opendcl.com/version/version.txt");
static const TCHAR gszVersionHostUrlDev[] =
	_T("https://www.opendcl.com/version/version_dev.txt");
static const CHAR gszDownloadActionUrl[] =
	"https://www.opendcl.com/download/";


struct UpdateCheckParams_t
{
	CStringA sProductName;
	CStringA sInstalledVersion;
	CStringA sLanguage;
	UpdateCheckParams_t( LPCTSTR pszProductName, LPCTSTR pszInstalledVersion, LPCTSTR pszLanguage )
		: sProductName( pszProductName ), sInstalledVersion( pszInstalledVersion ), sLanguage( pszLanguage ) {}
	UpdateCheckParams_t( const UpdateCheckParams_t& Src )
		: sProductName( Src.sProductName ), sInstalledVersion( Src.sInstalledVersion ), sLanguage( Src.sLanguage ) {}
};


struct UpdateNotificationParams_t
{
	CStringA sTitle;
	CStringA sMessage;
	CStringA sAction;
	UpdateNotificationParams_t( LPCSTR pszTitle, LPCSTR pszMessage, LPCSTR pszAction )
		: sTitle( pszTitle ), sMessage( pszMessage ), sAction( pszAction ) {}
	UpdateNotificationParams_t( const UpdateNotificationParams_t& Src )
		: sTitle( Src.sTitle ), sMessage( Src.sMessage ), sAction( Src.sAction ) {}
};


static bool CheckForUpdates( const UpdateCheckParams_t& Params, UpdateNotificationParams_t* pResponse = NULL );
static DWORD WINAPI BackgroundCheckForUpdates( LPVOID pvParam );
static DWORD WINAPI BackgroundUpdateNotification( LPVOID pvParam );
static LRESULT CALLBACK TrayIconWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
static ATOM trayIconWndClass();
static bool isUsingShellV1();
static bool isDevProductName( const CStringA& sProductName );
static bool httpGetText( LPCTSTR pszUrl, CStringA& sBody );
static CStringA sanitizeVersionString( CStringA sVersion );
static bool isFourPartVersion( const CStringA& sVersion );
static int compareFourPartVersions( const CStringA& sLeft, const CStringA& sRight );


ATOM trayIconWndClass()
{
	static ATOM wcTrayIcon = 0;
	if( !wcTrayIcon )
	{
		WNDCLASS stMsgWnd =
		{
			0,
			TrayIconWndProc,
			0,
			0,
			_hdllInstance,
			NULL,
			NULL,
			NULL,
			NULL,
			_T("OpenDCL.UpdateCheckTrayIconWnd")
		};
		wcTrayIcon = RegisterClass( &stMsgWnd );
	}
	return wcTrayIcon;
}


bool UpdateCheck( LPCTSTR pszProductName, LPCTSTR pszInstalledVersion /*N.N.N.N format*/ )
{
	UpdateCheckParams_t* pParams =
		new UpdateCheckParams_t( pszProductName, pszInstalledVersion, theWorkspace.GetLanguage() );
	DWORD dwThreadId;
	HANDLE hThread = CreateThread( NULL, 4096, BackgroundCheckForUpdates, pParams, 0, &dwThreadId );
	if( !hThread )
	{
		delete pParams;
		return false;
	}
	CloseHandle( hThread );
	return true;
}

bool isUsingShellV1()
{
	static bool bUsingShellV1 = false;
	static bool bAlreadySet = false;
	if( !bAlreadySet )
	{
		bAlreadySet = true;
		HMODULE hmodShell32 = LoadLibrary( _T("Shell32.dll") );
		if( hmodShell32 )
		{
			DWORD dwMajor;
			DWORD dwMinor;
			DWORD dwThird;
			DWORD dwFourth;
			if( theWorkspace.GetModuleVersionInfo( dwMajor, dwMinor, dwThird, dwFourth, hmodShell32 ) && dwMajor < 5 )
				bUsingShellV1 = true;
		}
		FreeLibrary( hmodShell32 );
	}
	return bUsingShellV1;
}

bool isDevProductName( const CStringA& sProductName )
{
	// acrxEntryPoint: "OpenDCL Runtime" (stable) vs "OpenDCL Runtime Dev"
	return sProductName.CompareNoCase( "OpenDCL Runtime Dev" ) == 0;
}

CStringA sanitizeVersionString( CStringA sVersion )
{
	sVersion.Trim( " \t\r\n" );
	// Keep only digits and dots (mirrors historical vercheck.php safe_version).
	CStringA sClean;
	for( int idx = 0; idx < sVersion.GetLength(); ++idx )
	{
		CHAR ch = sVersion[idx];
		if( ( ch >= '0' && ch <= '9' ) || ch == '.' )
			sClean += ch;
	}
	return sClean;
}

bool isFourPartVersion( const CStringA& sVersion )
{
	// Product versions are always A.B.C.D (exactly four numeric parts).
	// Reject "81" / "1.2" and digit soup extracted from HTML error pages.
	if( sVersion.IsEmpty() )
		return false;
	int nParts = 0;
	int nDigitsInPart = 0;
	for( int idx = 0; idx < sVersion.GetLength(); ++idx )
	{
		CHAR ch = sVersion[idx];
		if( ch >= '0' && ch <= '9' )
		{
			++nDigitsInPart;
		}
		else if( ch == '.' )
		{
			if( nDigitsInPart == 0 )
				return false;
			++nParts;
			nDigitsInPart = 0;
		}
		else
			return false;
	}
	if( nDigitsInPart == 0 )
		return false;
	++nParts;
	return nParts == 4;
}

int compareFourPartVersions( const CStringA& sLeft, const CStringA& sRight )
{
	// Component-wise numeric compare; missing parts treat as 0.
	int leftParts[4] = { 0, 0, 0, 0 };
	int rightParts[4] = { 0, 0, 0, 0 };
	int nLeft = 0;
	int nRight = 0;
	int nCur = 0;
	for( int idx = 0; idx <= sLeft.GetLength() && nLeft < 4; ++idx )
	{
		if( idx == sLeft.GetLength() || sLeft[idx] == '.' )
		{
			leftParts[nLeft++] = nCur;
			nCur = 0;
		}
		else if( sLeft[idx] >= '0' && sLeft[idx] <= '9' )
			nCur = nCur * 10 + ( sLeft[idx] - '0' );
	}
	nCur = 0;
	for( int idx = 0; idx <= sRight.GetLength() && nRight < 4; ++idx )
	{
		if( idx == sRight.GetLength() || sRight[idx] == '.' )
		{
			rightParts[nRight++] = nCur;
			nCur = 0;
		}
		else if( sRight[idx] >= '0' && sRight[idx] <= '9' )
			nCur = nCur * 10 + ( sRight[idx] - '0' );
	}
	for( int i = 0; i < 4; ++i )
	{
		if( leftParts[i] < rightParts[i] )
			return -1;
		if( leftParts[i] > rightParts[i] )
			return 1;
	}
	return 0;
}

bool httpGetText( LPCTSTR pszUrl, CStringA& sBody )
{
	sBody.Empty();
	HINTERNET hConnection = InternetOpen( _T("OpenDCL/1.0"),
																				INTERNET_OPEN_TYPE_PRECONFIG,
																				NULL,
																				NULL,
																				0 );
	if( !hConnection )
		return false;

	// InternetOpenUrl follows redirects (http→https, apex→www) and uses HTTPS when the URL says so.
	HINTERNET hRequest = InternetOpenUrl(
		hConnection,
		pszUrl,
		NULL,
		0,
		INTERNET_FLAG_RELOAD |
		INTERNET_FLAG_NO_CACHE_WRITE |
		INTERNET_FLAG_SECURE,
		0 );
	if( !hRequest )
	{
		InternetCloseHandle( hConnection );
		return false;
	}

	DWORD dwStatus = 0;
	DWORD cbStatus = sizeof( dwStatus );
	if( !HttpQueryInfo( hRequest,
											HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
											&dwStatus,
											&cbStatus,
											NULL ) ||
			dwStatus < 200 || dwStatus > 299 )
	{
		InternetCloseHandle( hRequest );
		InternetCloseHandle( hConnection );
		return false;
	}

	// Version files are a few bytes; cap reads so a large error body cannot fill memory.
	const DWORD kMaxBody = 64;
	CHAR szChunk[64];
	DWORD cbRead = 0;
	while( InternetReadFile( hRequest, szChunk, sizeof(szChunk) - 1, &cbRead ) && cbRead > 0 )
	{
		szChunk[cbRead] = 0;
		sBody += szChunk;
		if( (DWORD)sBody.GetLength() >= kMaxBody )
		{
			sBody = sBody.Left( kMaxBody );
			break;
		}
	}

	InternetCloseHandle( hRequest );
	InternetCloseHandle( hConnection );
	return !sBody.IsEmpty();
}

bool CheckForUpdates( const UpdateCheckParams_t& Params, UpdateNotificationParams_t* pResponse /*= NULL*/ )
{
	bool bFailed = true;
	try
	{
		const TCHAR* pszUrl = isDevProductName( Params.sProductName )
			? gszVersionHostUrlDev
			: gszVersionHostUrlStable;

		CStringA sInstalled = sanitizeVersionString( Params.sInstalledVersion );
		CStringA sLatestRaw;
		if( !httpGetText( pszUrl, sLatestRaw ) )
			return false;

		// Do not digit-strip the response: that turns HTML (e.g. viewport "1") into
		// fake versions like "81". Accept only a trimmed plain A.B.C.D body.
		CStringA sLatest = sLatestRaw;
		sLatest.Trim( " \t\r\n" );
		if( !isFourPartVersion( sLatest ) )
			return false;

		if( !isFourPartVersion( sInstalled ) )
			sInstalled = "0.0.0.0";

		if( compareFourPartVersions( sInstalled, sLatest ) >= 0 )
		{
			// Installed version is current (or newer) — same as empty PHP response.
			if( pResponse )
			{
				pResponse->sTitle.Empty();
				pResponse->sMessage.Empty();
				pResponse->sAction.Empty();
			}
			bFailed = false;
		}
		else
		{
			CStringA sTitle = "OpenDCL Update Available";
			CStringA sMessage;
			sMessage.Format(
				"A newer version (%s) of OpenDCL is available. Click here to download the current version.",
				(LPCSTR)sLatest );
			CStringA sAction = gszDownloadActionUrl;

			if( pResponse )
			{
				pResponse->sTitle = sTitle;
				pResponse->sMessage = sMessage;
				pResponse->sAction = sAction;
				bFailed = false;
			}
			else
			{
				UpdateNotificationParams_t* pParams =
					new UpdateNotificationParams_t( sTitle, sMessage, sAction );
				DWORD dwThreadId;
				HANDLE hThread = CreateThread( NULL, 4096, BackgroundUpdateNotification, pParams, 0, &dwThreadId );
				if( !hThread )
					delete pParams;
				else
				{
					CloseHandle( hThread );
					bFailed = false;
				}
			}
		}
	}
	catch( ... )
	{}
	return !bFailed;
}

DWORD WINAPI BackgroundCheckForUpdates( LPVOID pvParam )
{
	UpdateCheckParams_t Params( *(UpdateCheckParams_t*)pvParam );
	delete (UpdateCheckParams_t*)pvParam;
	CheckForUpdates( Params );
	return 0;
}


DWORD WINAPI BackgroundUpdateNotification( LPVOID pvParam )
{
	HWND hwndAcad = adsw_acadMainWnd();
	HWND hwnd = CreateWindow( (LPCTSTR)trayIconWndClass(), NULL, WS_ICONIC | WS_CHILD, 0, 0, 0, 0, hwndAcad, NULL, _hdllInstance, pvParam );
	static class _autohwnd //to make sure the tray icon is destroyed when this module is unloaded
	{
		std::set< HWND > hwnds;
	public:
		_autohwnd() {}
		~_autohwnd() { clear( false ); }
		void clear( bool bDestroy = true )
			{
				for( std::set< HWND >::iterator iter = hwnds.begin(); iter != hwnds.end(); ++iter )
				{
					NOTIFYICONDATA NI;
					ZeroMemory( &NI, sizeof(NI) );
				#ifdef NOTIFYICONDATA_V1_SIZE
					NI.cbSize = (isUsingShellV1()? NOTIFYICONDATA_V1_SIZE : sizeof(NOTIFYICONDATA));
				#else
					NI.cbSize = sizeof(NOTIFYICONDATA);
				#endif //NOTIFYICONDATA_V1_SIZE
					NI.hWnd = *iter;
					NI.uID = 1;
					Shell_NotifyIcon( NIM_DELETE, &NI );
					if( bDestroy )
						PostMessage( *iter, WM_QUIT, 0, 0 );
				}
				hwnds.clear();
			}
		void add( HWND hwnd )
			{
				clear();
				hwnds.insert( hwnd );
			}
		void remove( HWND hwnd )
			{
				hwnds.erase( hwnd );
			}
	} autohwnd;
	autohwnd.add( hwnd );
	bool bQuit = false;
	while( !bQuit )
	{
		MSG msg;
		while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			if( msg.message == WM_QUIT )
				bQuit = true;
		}
		if( !bQuit )
			WaitMessage();
	}
	autohwnd.remove( hwnd );
	return 0;
}


LRESULT CALLBACK TrayIconWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static bool bAUCEnabled = true;
	static NOTIFYICONDATA NI;
	switch( uMsg )
	{
		case WM_QUIT:
		{
			DestroyWindow( hwnd );
			return 0;
		}
		case WM_NCCREATE:
		{
			return (LRESULT)TRUE;
		}
		case WM_CREATE:
		{
			//store the window's parameters
			CREATESTRUCT* pCS = (CREATESTRUCT*)lParam;
			SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR)pCS->lpCreateParams );
			UpdateNotificationParams_t* pParams = (UpdateNotificationParams_t*)GetWindowLongPtr( hwnd, GWLP_USERDATA );
			ZeroMemory( &NI, sizeof(NI) );
		#ifdef NOTIFYICONDATA_V1_SIZE
			NI.cbSize = (isUsingShellV1()? NOTIFYICONDATA_V1_SIZE : sizeof(NOTIFYICONDATA));
		#else
			NI.cbSize = sizeof(NOTIFYICONDATA);
		#endif //NOTIFYICONDATA_V1_SIZE
			NI.hWnd = hwnd;
			NI.uID = 1;
			NI.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			NI.uCallbackMessage = WM_MMTRAY_NOTIFY;
			NI.hIcon = LoadIcon( theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE(IDR_MAINFRAME) );
			lstrcpyn( NI.szTip, CString( pParams->sMessage ), _elements(NI.szTip) );
		#if (_WIN32_WINNT >= 0x0500)
			if( !isUsingShellV1() )
			{
				NI.uFlags |= NIF_INFO;
				lstrcpyn( NI.szInfo, CString( pParams->sMessage ), _elements(NI.szInfo) );
				lstrcpyn( NI.szInfoTitle, CString( pParams->sTitle ), _elements(NI.szInfoTitle) );
				NI.dwInfoFlags = NIIF_INFO;
				NI.uTimeout = 3000;
			}
		#endif
			Shell_NotifyIcon( NIM_ADD, &NI );
		#if (_WIN32_WINNT >= 0x0500)
			if( !isUsingShellV1() )
			{
				UINT nTimeout = NI.uTimeout;
				NI.uVersion = 6;
				Shell_NotifyIcon( NIM_SETVERSION, &NI );
				NI.uTimeout = nTimeout;
			}
		#endif
			break;
		}
		case WM_DESTROY:
		{
			ZeroMemory( &NI, sizeof(NI) );
		#ifdef NOTIFYICONDATA_V1_SIZE
			NI.cbSize = (isUsingShellV1()? NOTIFYICONDATA_V1_SIZE : sizeof(NOTIFYICONDATA));
		#else
			NI.cbSize = sizeof(NOTIFYICONDATA);
		#endif //NOTIFYICONDATA_V1_SIZE
			NI.hWnd = hwnd;
			NI.uID = 1;
			Shell_NotifyIcon( NIM_DELETE, &NI );
			delete (UpdateNotificationParams_t*)GetWindowLongPtr( hwnd, GWLP_USERDATA );
			break;
		}
		case WM_ACTIVATEAPP:
		{
		#ifdef NIN_BALLOONUSERCLICK
			if( !((UpdateNotificationParams_t*)GetWindowLongPtr( hwnd, GWLP_USERDATA ))->sAction.IsEmpty() )
				SendMessage( hwnd, WM_MMTRAY_NOTIFY, 0, NIN_BALLOONUSERCLICK );
		#endif //NIN_BALLOONUSERCLICK
			break;
		}
		case WM_MMTRAY_NOTIFY:
		{
			switch( lParam )
			{
			#ifdef NIN_BALLOONUSERCLICK
				case NIN_BALLOONUSERCLICK:
			#endif //NIN_BALLOONUSERCLICK
				case WM_LBUTTONDOWN:
				{
					CString sAction( ((UpdateNotificationParams_t*)GetWindowLongPtr( hwnd, GWLP_USERDATA ))->sAction );
					if( !sAction.IsEmpty() )
						ShellExecute( NULL, _T("open"), sAction, NULL, NULL, SW_SHOWNORMAL );
					PostMessage( hwnd, WM_QUIT, 0, 0 );
					return 0;
					break;
				}
				case WM_RBUTTONDOWN:
				{
					HMENU hContextMenu = CreatePopupMenu();
					if( hContextMenu )
					{
						HWND hwndParent = ::GetParent( hwnd );
						SetForegroundWindow( hwndParent );
						PostMessage( hwndParent, WM_NULL, 0, 0 );
						bAUCEnabled = theWorkspace.IsAutoUpdateCheckEnabled();
						AppendMenu( hContextMenu,
												MF_STRING | (bAUCEnabled? MF_CHECKED : MF_UNCHECKED),
												ID_TOGGLEUPDATECHECK,
												(LPCTSTR)theWorkspace.LoadResourceString( IDS_AUTOUPDATECHECK ) );
						AppendMenu( hContextMenu,
												MF_SEPARATOR,
												-1,
												NULL );
						AppendMenu( hContextMenu,
												MF_STRING | MF_UNCHECKED,
												-1,
												(LPCTSTR)theWorkspace.LoadResourceString( IDS_CLOSEMENU ) );
						POINT ptCursor;
						GetCursorPos( &ptCursor );
						TrackPopupMenu( hContextMenu,
														TPM_LEFTALIGN,
														ptCursor.x,
														ptCursor.y,
														0,
														hwnd,
														NULL );
						DestroyMenu( hContextMenu );
					}
					break;
				}
			};
			break;
		}
		case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
				case ID_TOGGLEUPDATECHECK:
				{
					bAUCEnabled = !bAUCEnabled;
					theWorkspace.SetAutoUpdateCheckEnabled( bAUCEnabled );
					break;
				}
			};
			break;
		}
	};
	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}
