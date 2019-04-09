#include "stdafx.h"
#include "DpiAwarenessAPI.h"

// Copyright 2017 - 2019 ManuSoft. All Rights Reserved.
// http://www.manusoft.com
//
// A license to use the code in this file for the OpenDCL project has been granted
// by Owen Wengerd and ManuSoft. This code is provided under the terms of the GPL
// license governing OpenDCL. This code may not be used for any other purpose
// without the express written consent of Owen Wengerd or ManuSoft.


class DpiAPImportTable
{
	bool mbInitialized;
	HMODULE mhmodUser32;
	HMODULE mhmodShcore;

public:
	DpiAPImportTable() : mbInitialized( false ), mhmodUser32( NULL ), mhmodShcore( NULL ) {}
	virtual ~DpiAPImportTable() { FreeLibrary( mhmodUser32 ); FreeLibrary( mhmodShcore ); }

protected:
	bool check(void*& pfTarget, LPCSTR pszFuncName)
	{
		if( pfTarget )
			return true;
		if( !mbInitialized )
		{
			mbInitialized = true;
			mhmodUser32 = LoadLibrary( _T("User32.dll") );
			mhmodShcore = LoadLibrary( _T("Shcore.dll") );
		}
		if( mhmodUser32 )
			pfTarget = GetProcAddress( mhmodUser32, pszFuncName );
		if( !pfTarget && mhmodShcore )
			pfTarget = GetProcAddress( mhmodShcore, pszFuncName );
		return (pfTarget != NULL);
	}

private:
	// API function type declarations
	typedef HMONITOR (STDAPICALLTYPE *F_MonitorFromPoint)( POINT pt, DWORD dwFlags );
	typedef HRESULT (STDAPICALLTYPE *F_GetDpiForMonitor)( HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY );
	typedef UINT (STDAPICALLTYPE *F_GetDpiForSystem)();
	typedef UINT (STDAPICALLTYPE *F_GetDpiForWindow)( HWND hwnd );
	typedef HRESULT (STDAPICALLTYPE *F_GetProcessDpiAwareness)( HANDLE hprocess, PROCESS_DPI_AWARENESS* value );
	typedef int (STDAPICALLTYPE *F_GetSystemMetricsForDpi)( int nIndex, UINT dpi );
	typedef DPI_AWARENESS_CONTEXT (STDAPICALLTYPE *F_GetThreadDpiAwarenessContext)();
	typedef DPI_AWARENESS_CONTEXT (STDAPICALLTYPE *F_GetWindowDpiAwarenessContext)( HWND hwnd );
	typedef BOOL (STDAPICALLTYPE *F_IsValidDpiAwarenessContext)( DPI_AWARENESS_CONTEXT value );
	typedef BOOL (STDAPICALLTYPE *F_LogicalToPhysicalPoint)( HWND hwnd, LPPOINT lpPoint );
	typedef BOOL (STDAPICALLTYPE *F_LogicalToPhysicalPointForPerMonitorDPI)( HWND hwnd, LPPOINT lpPoint );
	typedef BOOL (STDAPICALLTYPE *F_PhysicalToLogicalPoint)( HWND hwnd, LPPOINT lpPoint );
	typedef BOOL (STDAPICALLTYPE *F_PhysicalToLogicalPointForPerMonitorDPI)( HWND hwnd, LPPOINT lpPoint );
	typedef DPI_AWARENESS (STDAPICALLTYPE *F_GetAwarenessFromDpiAwarenessContext)( DPI_AWARENESS_CONTEXT value );
	typedef BOOL (STDAPICALLTYPE *F_SystemParametersInfoForDpi)( UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi );
	typedef HTHEME (STDAPICALLTYPE *F_OpenThemeDataForDpi)( HWND hwnd, PCWSTR pszClassIdList, UINT dpi );
	typedef BOOL (STDAPICALLTYPE *F_AdjustWindowRectExForDpi)( LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi );
	typedef BOOL (STDAPICALLTYPE *F_SetDialogDpiChangeBehavior)( HWND hDlg, DIALOG_DPI_CHANGE_BEHAVIORS mask, DIALOG_DPI_CHANGE_BEHAVIORS values );
	typedef DIALOG_DPI_CHANGE_BEHAVIORS (STDAPICALLTYPE *F_GetDialogDpiChangeBehavior)( HWND hDlg );
	typedef BOOL (STDAPICALLTYPE *F_AreDpiAwarenessContextsEqual)( DPI_AWARENESS_CONTEXT dpiContextA, DPI_AWARENESS_CONTEXT dpiContextB );
	typedef DPI_AWARENESS_CONTEXT (STDAPICALLTYPE *F_SetThreadDpiAwarenessContext)( DPI_AWARENESS_CONTEXT dpiContext );
	typedef BOOL (STDAPICALLTYPE *F_SetProcessDpiAwarenessContext)( DPI_AWARENESS_CONTEXT dpiContext );
	typedef HRESULT (STDAPICALLTYPE *F_SetProcessDpiAwareness)( PROCESS_DPI_AWARENESS value );

	//auto-init pointer to NULL
	template< typename FType > class FPtr
	{
		FType mpfTarget;
	public:
		FPtr() : mpfTarget( NULL ) {}
		operator FType() const { return mpfTarget; }
		operator void*& () const { return *(void**)&mpfTarget; }
		FType asFPtr() const { return mpfTarget; }
	};


	// API function pointers
	FPtr< F_MonitorFromPoint > mpfMonitorFromPoint;
	FPtr< F_GetDpiForMonitor > mpfGetDpiForMonitor;
	FPtr< F_GetDpiForSystem > mpfGetDpiForSystem;
	FPtr< F_GetProcessDpiAwareness > mpfGetProcessDpiAwareness;
	FPtr< F_GetDpiForWindow > mpfGetDpiForWindow;
	FPtr< F_GetSystemMetricsForDpi > mpfGetSystemMetricsForDpi;
	FPtr< F_GetThreadDpiAwarenessContext > mpfGetThreadDpiAwarenessContext;
	FPtr< F_GetWindowDpiAwarenessContext > mpfGetWindowDpiAwarenessContext;
	FPtr< F_IsValidDpiAwarenessContext > mpfIsValidDpiAwarenessContext;
	FPtr< F_LogicalToPhysicalPoint > mpfLogicalToPhysicalPoint;
	FPtr< F_LogicalToPhysicalPointForPerMonitorDPI > mpfLogicalToPhysicalPointForPerMonitorDPI;
	FPtr< F_PhysicalToLogicalPoint > mpfPhysicalToLogicalPoint;
	FPtr< F_PhysicalToLogicalPointForPerMonitorDPI > mpfPhysicalToLogicalPointForPerMonitorDPI;
	FPtr< F_GetAwarenessFromDpiAwarenessContext > mpfGetAwarenessFromDpiAwarenessContext;
	FPtr< F_SystemParametersInfoForDpi > mpfSystemParametersInfoForDpi;
	FPtr< F_OpenThemeDataForDpi > mpfOpenThemeDataForDpi;
	FPtr< F_AdjustWindowRectExForDpi > mpfAdjustWindowRectExForDpi;
	FPtr< F_SetDialogDpiChangeBehavior > mpfSetDialogDpiChangeBehavior;
	FPtr< F_GetDialogDpiChangeBehavior > mpfGetDialogDpiChangeBehavior;
	FPtr< F_AreDpiAwarenessContextsEqual > mpfAreDpiAwarenessContextsEqual;
	FPtr< F_SetThreadDpiAwarenessContext > mpfSetThreadDpiAwarenessContext;
	FPtr< F_SetProcessDpiAwarenessContext > mpfSetProcessDpiAwarenessContext;
	FPtr< F_SetProcessDpiAwareness > mpfSetProcessDpiAwareness;


	#define CHECK(name, onfail) return (!check(mpf##name,#name))? (onfail) : mpf##name.asFPtr()

public:
	// API function stubs
	HMONITOR MonitorFromPoint( POINT pt, DWORD dwFlags ) { CHECK(MonitorFromPoint, NULL)( pt, dwFlags ); }
	HRESULT GetDpiForMonitor( HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY ) { CHECK(GetDpiForMonitor, E_NOTIMPL)( hmonitor, dpiType, dpiX, dpiY ); }
	UINT GetDpiForSystem() { CHECK(GetDpiForSystem, 96)(); }
	HRESULT GetProcessDpiAwareness( HANDLE hprocess, PROCESS_DPI_AWARENESS* value ) { CHECK(GetProcessDpiAwareness, E_NOTIMPL)( hprocess, value ); }
	UINT GetDpiForWindow( HWND hwnd ) { CHECK(GetDpiForWindow, 96)( hwnd ); }
	int GetSystemMetricsForDpi( int nIndex, UINT dpi ) { CHECK(GetSystemMetricsForDpi, 0)( nIndex, dpi ); }
	DPI_AWARENESS_CONTEXT GetThreadDpiAwarenessContext() { CHECK(GetThreadDpiAwarenessContext, DPI_AWARENESS_CONTEXT_UNAWARE)(); }
	DPI_AWARENESS_CONTEXT GetWindowDpiAwarenessContext( HWND hwnd ) { CHECK(GetWindowDpiAwarenessContext, DPI_AWARENESS_CONTEXT_UNAWARE)( hwnd ); }
	BOOL IsValidDpiAwarenessContext( DPI_AWARENESS_CONTEXT value ) { CHECK(IsValidDpiAwarenessContext, FALSE)( value ); }
	BOOL LogicalToPhysicalPoint( HWND hwnd, LPPOINT lpPoint ) { CHECK(LogicalToPhysicalPoint, FALSE)( hwnd, lpPoint ); }
	BOOL LogicalToPhysicalPointForPerMonitorDPI( HWND hwnd, LPPOINT lpPoint ) { CHECK(LogicalToPhysicalPoint, FALSE)( hwnd, lpPoint ); }
	BOOL PhysicalToLogicalPoint( HWND hwnd, LPPOINT lpPoint ) { CHECK(LogicalToPhysicalPoint, FALSE)( hwnd, lpPoint ); }
	BOOL PhysicalToLogicalPointForPerMonitorDPI( HWND hwnd, LPPOINT lpPoint ) { CHECK(LogicalToPhysicalPoint, FALSE)( hwnd, lpPoint ); }
	DPI_AWARENESS GetAwarenessFromDpiAwarenessContext( DPI_AWARENESS_CONTEXT value ) { CHECK(GetAwarenessFromDpiAwarenessContext, DPI_AWARENESS_UNAWARE)( value ); }
	BOOL SystemParametersInfoForDpi( UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi ) { CHECK(SystemParametersInfoForDpi, FALSE)( uiAction, uiParam, pvParam, fWinIni, dpi ); }
	HTHEME OpenThemeDataForDpi( HWND hwnd, PCWSTR pszClassIdList, UINT dpi ) { CHECK(OpenThemeDataForDpi, NULL)( hwnd, pszClassIdList, dpi ); }
	BOOL AdjustWindowRectExForDpi( LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi ) { CHECK(AdjustWindowRectExForDpi, FALSE)( lpRect, dwStyle, bMenu, dwExStyle, dpi ); }
	BOOL SetDialogDpiChangeBehavior( HWND hDlg, DIALOG_DPI_CHANGE_BEHAVIORS mask, DIALOG_DPI_CHANGE_BEHAVIORS values ) { CHECK(SetDialogDpiChangeBehavior, FALSE)( hDlg, mask, values ); }
	DIALOG_DPI_CHANGE_BEHAVIORS GetDialogDpiChangeBehavior( HWND hDlg ) { CHECK(GetDialogDpiChangeBehavior, DDC_DEFAULT)( hDlg ); }
	BOOL AreDpiAwarenessContextsEqual( DPI_AWARENESS_CONTEXT dpiContextA, DPI_AWARENESS_CONTEXT dpiContextB ) { CHECK(AreDpiAwarenessContextsEqual, FALSE)( dpiContextA, dpiContextB ); }
	DPI_AWARENESS_CONTEXT SetThreadDpiAwarenessContext( DPI_AWARENESS_CONTEXT dpiContext ) { CHECK(SetThreadDpiAwarenessContext, DPI_AWARENESS_CONTEXT_UNAWARE)( dpiContext ); }
	BOOL SetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT dpiContext ) { CHECK(SetProcessDpiAwarenessContext, FALSE)( dpiContext ); }
	HRESULT SetProcessDpiAwareness( PROCESS_DPI_AWARENESS value ) { CHECK(SetProcessDpiAwareness, E_NOTIMPL)( value ); }
};


// To enable delayed initialization
static DpiAPImportTable& getDPI()
{
	static DpiAPImportTable theDpiAPImportTable;
	return theDpiAPImportTable;
}
#define DPI getDPI()

// Dpi Aware API functions

HMONITOR DpiAwarenessHelper::MonitorFromPoint( POINT pt, DWORD dwFlags )
{
	return DPI.MonitorFromPoint( pt, dwFlags );
}

HRESULT DpiAwarenessHelper::GetDpiForMonitor( HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY )
{
	return DPI.GetDpiForMonitor( hmonitor, dpiType, dpiX, dpiY );
}

UINT DpiAwarenessHelper::GetDpiForSystem()
{
	return DPI.GetDpiForSystem();
}

UINT DpiAwarenessHelper::GetDpiForWindow( HWND hwnd )
{
	return DPI.GetDpiForWindow( hwnd );
}

HRESULT DpiAwarenessHelper::GetProcessDpiAwareness( HANDLE hprocess, PROCESS_DPI_AWARENESS* value )
{
	return DPI.GetProcessDpiAwareness( hprocess, value );
}

int DpiAwarenessHelper::GetSystemMetricsForDpi( int nIndex, UINT dpi )
{
	return DPI.GetSystemMetricsForDpi( nIndex, dpi );
}

DPI_AWARENESS_CONTEXT DpiAwarenessHelper::GetThreadDpiAwarenessContext()
{
	return DPI.GetThreadDpiAwarenessContext();
}

DPI_AWARENESS_CONTEXT DpiAwarenessHelper::GetWindowDpiAwarenessContext( HWND hwnd )
{
	return DPI.GetWindowDpiAwarenessContext( hwnd );
}

BOOL DpiAwarenessHelper::IsValidDpiAwarenessContext( DPI_AWARENESS_CONTEXT value )
{
	return DPI.IsValidDpiAwarenessContext( value );
}

BOOL DpiAwarenessHelper::LogicalToPhysicalPoint( HWND hwnd, LPPOINT lpPoint )
{
	return DPI.LogicalToPhysicalPoint( hwnd, lpPoint );
}

BOOL DpiAwarenessHelper::LogicalToPhysicalPointForPerMonitorDPI( HWND hwnd, LPPOINT lpPoint )
{
	return DPI.LogicalToPhysicalPointForPerMonitorDPI( hwnd, lpPoint );
}

BOOL DpiAwarenessHelper::PhysicalToLogicalPoint( HWND hwnd, LPPOINT lpPoint )
{
	return DPI.PhysicalToLogicalPoint( hwnd, lpPoint );
}

BOOL DpiAwarenessHelper::PhysicalToLogicalPointForPerMonitorDPI( HWND hwnd, LPPOINT lpPoint )
{
	return DPI.PhysicalToLogicalPointForPerMonitorDPI( hwnd, lpPoint );
}

DPI_AWARENESS DpiAwarenessHelper::GetAwarenessFromDpiAwarenessContext( DPI_AWARENESS_CONTEXT value )
{
	return DPI.GetAwarenessFromDpiAwarenessContext( value );
}

BOOL DpiAwarenessHelper::SystemParametersInfoForDpi( UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi )
{
	return DPI.SystemParametersInfoForDpi( uiAction, uiParam, pvParam, fWinIni, dpi );
}

HTHEME DpiAwarenessHelper::OpenThemeDataForDpi( HWND hwnd, PCWSTR pszClassIdList, UINT dpi )
{
	return DPI.OpenThemeDataForDpi( hwnd, pszClassIdList, dpi );
}

BOOL DpiAwarenessHelper::AdjustWindowRectExForDpi( LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi )
{
	return DPI.AdjustWindowRectExForDpi( lpRect, dwStyle, bMenu, dwExStyle, dpi );
}

BOOL DpiAwarenessHelper::SetDialogDpiChangeBehavior( HWND hDlg, DIALOG_DPI_CHANGE_BEHAVIORS mask, DIALOG_DPI_CHANGE_BEHAVIORS values )
{
	return DPI.SetDialogDpiChangeBehavior( hDlg, mask, values );
}

DIALOG_DPI_CHANGE_BEHAVIORS DpiAwarenessHelper::GetDialogDpiChangeBehavior( HWND hDlg )
{
	return DPI.GetDialogDpiChangeBehavior( hDlg );
}

BOOL DpiAwarenessHelper::AreDpiAwarenessContextsEqual( DPI_AWARENESS_CONTEXT dpiContextA, DPI_AWARENESS_CONTEXT dpiContextB )
{
	return DPI.AreDpiAwarenessContextsEqual( dpiContextA, dpiContextB );
}

DPI_AWARENESS_CONTEXT DpiAwarenessHelper::SetThreadDpiAwarenessContext( DPI_AWARENESS_CONTEXT dpiContext )
{
	return DPI.SetThreadDpiAwarenessContext( dpiContext );
}

BOOL DpiAwarenessHelper::SetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT dpiContext )
{
	return DPI.SetProcessDpiAwarenessContext( dpiContext );
}

HRESULT DpiAwarenessHelper::SetProcessDpiAwareness( PROCESS_DPI_AWARENESS value )
{
	return DPI.SetProcessDpiAwareness( value );
}


// Utility functions

bool DpiAwareness::FromDIP( LONG* values, USHORT count ) const
{
	int nDPI = (int)GetDpi();
	if ( nDPI != 96 )
	{
		while ( count-- > 0 )
		{
			*values = MulDiv( *values, nDPI, 96 );
			++values;
		}
	}
	return true;
}

bool DpiAwareness::ToDIP( LONG* values, USHORT count ) const
{
	int nDPI = (int)GetDpi();
	if ( nDPI != 96 )
	{
		while ( count-- > 0 )
		{
			*values = MulDiv( *values, 96, nDPI );
			++values;
		}
	}
	return true;
}

bool DpiAwareness::IsProcessDpiAware( HANDLE hprocess )
{
	PROCESS_DPI_AWARENESS value;
	if (FAILED(GetProcessDpiAwareness( hprocess, &value )))
		return false;
	return (value > PROCESS_DPI_UNAWARE);
}

bool DpiAwareness::IsDpiAwarePerMonitorV2( HWND hwnd )
{
	return (AreDpiAwarenessContextsEqual( GetWindowDpiAwarenessContext( hwnd ), DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ) == TRUE);
}
