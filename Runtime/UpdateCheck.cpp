// UpdateCheck.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateCheck.h"
#include "Workspace.h"
#include "SharedRes.h"
#include "Resource.h"
#include <memory>
#include <set>
#include <WinINet.h>
#include <ShellAPI.h>

#pragma comment( lib, "Wininet.lib" )


static const UINT WM_MMTRAY_NOTIFY = WM_USER + 50;
static const UINT ID_TOGGLEUPDATECHECK = WM_USER + 51;


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


static LPCSTR constructPostData( const UpdateCheckParams_t& Params );
static DWORD WINAPI BackgroundCheckForUpdates( LPVOID pvParam );
static DWORD WINAPI BackgroundUpdateNotification( LPVOID pvParam );
static LRESULT CALLBACK TrayIconWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
static ATOM trayIconWndClass();
static CStringA UrlEncode( CStringA sUnsafe );
static bool isUsingShellV1();


class CTrayIconWnd
{
	HICON mhIcon;
	CString msNotification;
	CString msBubbleAction;
	HWND mhWnd;
	NOTIFYICONDATA mstNI;
	bool mbUsingShellV1;
	bool mbDeleting;
public:
	CTrayIconWnd();
	virtual ~CTrayIconWnd();
	HWND window() const { return mhWnd; }
	LRESULT ProcessMessage( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	NOTIFYICONDATA& GetNIStruct( bool& bUsingShellV1 );
	void SetIcon( HICON hIcon );
};


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

LPCSTR constructPostData( const UpdateCheckParams_t& Params )
{
	static CStringA sEncodedData;
	sEncodedData.Empty();
	if( !Params.sProductName.IsEmpty() )
	{
		if( !sEncodedData.IsEmpty() )
			sEncodedData += '&';
		sEncodedData += "productName=";
		sEncodedData += UrlEncode( Params.sProductName );
	}
	if( !Params.sInstalledVersion.IsEmpty() )
	{
		if( !sEncodedData.IsEmpty() )
			sEncodedData += '&';
		sEncodedData += "userVersion=";
		sEncodedData += Params.sInstalledVersion;
	}
	if( !Params.sLanguage.IsEmpty() )
	{
		if( !sEncodedData.IsEmpty() )
			sEncodedData += '&';
		sEncodedData += "language=";
		sEncodedData += Params.sLanguage;
	}
	return sEncodedData;
}

CStringA UrlEncode( CStringA sUnsafe )
{
	CStringA sSafe;
	for( int idx = 0; idx < sUnsafe.GetLength(); ++idx )
	{
		CHAR chThis = sUnsafe[idx];
		if( _istalnum( chThis ) )
			sSafe += chThis;
		else
		{
			CStringA sEnc;
			sEnc.Format( "%%%02X", chThis );
			sSafe += sEnc;
		}
	}
	return sSafe;
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

DWORD WINAPI BackgroundCheckForUpdates( LPVOID pvParam )
{
	UpdateCheckParams_t Params( *(UpdateCheckParams_t*)pvParam );
	delete (UpdateCheckParams_t*)pvParam;
	bool bFailed = true;
	try
	{
		HINTERNET hConnection = InternetOpen( _T("OpenDCL/1.0"),
																					INTERNET_OPEN_TYPE_PRECONFIG,
																					NULL,
																					NULL,
																					0 );
		if( hConnection )
		{
			HINTERNET hSession = InternetConnect( hConnection,
																						_T("opendcl.com"),
																						INTERNET_DEFAULT_HTTP_PORT,
																						_T(""),
																						_T(""),
																						INTERNET_SERVICE_HTTP,
																						0,
																						1 );
			if( hSession )
			{
				LPCTSTR rszAcceptType[] = { _T("text/*"), NULL };
				HINTERNET hRequest = HttpOpenRequest( hSession,
																							_T("POST"),
																							_T("/version/vercheck.php"),
																							NULL,
																							NULL,
																							rszAcceptType,
																							(INTERNET_FLAG_NO_CACHE_WRITE |
																							 INTERNET_FLAG_RELOAD /*|
																							 INTERNET_FLAG_SECURE |
																							 INTERNET_FLAG_IGNORE_CERT_DATE_INVALID*/), //ignore date, else Win95 fails
																							1 );
				if( hRequest )
				{
					CStringA sData = constructPostData( Params );
					TCHAR szHeaders[] = _T("Content-Type: application/x-www-form-urlencoded");
					if( HttpSendRequest( hRequest,
															 szHeaders, lstrlen( szHeaders ),
															 sData.LockBuffer(), sData.GetLength() ) )
					{
						DWORD cbRead;
						CStringA sResponse;
						if( InternetReadFile( hRequest, sResponse.GetBuffer( 512 ), 511, &cbRead ) )
						{
							sResponse.ReleaseBufferSetLength( cbRead );
							if( !sResponse.IsEmpty() )
							{
								//If the response is not empty, it is assumed to be in a pseudo-XML format
								static const CHAR szAttrNotification[] = "<Notification>";
								static const CHAR szAttrNotificationEnd[] = "</Notification>";
								int idxNotification = sResponse.Find( szAttrNotification );
								if( idxNotification >= 0 )
								{
									CStringA sNotification = sResponse.Mid( idxNotification + _elements( szAttrNotification ) - 1 );
									int idxNotificationEnd = sNotification.Find( szAttrNotificationEnd );
									if( idxNotificationEnd >= 0 )
										sNotification = sNotification.Left( idxNotificationEnd );
									static const CHAR szAttrTitle[] = "<Title>";
									static const CHAR szAttrTitleEnd[] = "</Title>";
									static const CHAR szAttrMessage[] = "<Message>";
									static const CHAR szAttrMessageEnd[] = "</Message>";
									static const CHAR szAttrAction[] = "<Action>";
									static const CHAR szAttrActionEnd[] = "</Action>";
									CStringA sTitle;
									CStringA sMessage;
									CStringA sAction;
									int idxTitle = sNotification.Find( szAttrTitle );
									if( idxTitle >= 0 )
									{
										sTitle = sNotification.Mid( idxTitle + _elements( szAttrTitle ) - 1 );
										int idxTitleEnd = sTitle.Find( szAttrTitleEnd );
										if( idxTitleEnd >= 0 )
											sTitle = sTitle.Left( idxTitleEnd );
									}
									int idxMessage = sNotification.Find( szAttrMessage );
									if( idxMessage >= 0 )
									{
										sMessage = sNotification.Mid( idxMessage + _elements( szAttrMessage ) - 1 );
										int idxMessageEnd = sMessage.Find( szAttrMessageEnd );
										if( idxMessageEnd >= 0 )
											sMessage = sMessage.Left( idxMessageEnd );
									}
									int idxAction = sNotification.Find( szAttrAction );
									if( idxAction >= 0 )
									{
										sAction = sNotification.Mid( idxAction + _elements( szAttrAction ) - 1 );
										int idxActionEnd = sAction.Find( szAttrActionEnd );
										if( idxActionEnd >= 0 )
											sAction = sAction.Left( idxActionEnd );
									}
									UpdateNotificationParams_t* pParams = new UpdateNotificationParams_t( sTitle, sMessage, sAction );
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
					}
					InternetCloseHandle( hRequest );
				}
				InternetCloseHandle( hSession );
			}
			InternetCloseHandle( hConnection );
		}
	}
	catch( ... )
	{}
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
			NI.hIcon = LoadIcon( _hdllInstance, MAKEINTRESOURCE(IDR_MAINFRAME) );
			lstrcpyn( NI.szTip, CString( pParams->sMessage ), _elements(NI.szTip) );
		#if (_WIN32_IE >= 0x0500)
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
		#if (_WIN32_IE >= 0x0500)
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
			Shell_NotifyIcon( NIM_DELETE, &NI );
			delete (UpdateNotificationParams_t*)GetWindowLongPtr( hwnd, GWLP_USERDATA );
			break;
		}
		case WM_ACTIVATEAPP:
		{
		#if (_WIN32_IE >= 0x0501)
			if( !((UpdateNotificationParams_t*)GetWindowLongPtr( hwnd, GWLP_USERDATA ))->sAction.IsEmpty() )
				SendMessage( hwnd, WM_MMTRAY_NOTIFY, 0, NIN_BALLOONUSERCLICK );
		#endif //(_WIN32_IE >= 0x0501)
			break;
		}
		case WM_MMTRAY_NOTIFY:
		{
			switch( lParam )
			{
			#if (_WIN32_IE >= 0x0501)
				case NIN_BALLOONUSERCLICK:
			#endif //(_WIN32_IE >= 0x0501)
				case WM_LBUTTONDOWN:
				{
					CString sAction = ((UpdateNotificationParams_t*)GetWindowLongPtr( hwnd, GWLP_USERDATA ))->sAction;
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
