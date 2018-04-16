#pragma once

// These two classes expose and abstract UxTheme API functions without an implicit reference to 
// UxTheme.dll. When API functions are called, the calls are passed through to the real functions 
// if they are available; else they return E_NOTIMPL or default value (e.g. when running on an 
// earlier version of Windows).
//
// Copyright 2017 - 2018 ManuSoft. All Rights Reserved.
// http://www.manusoft.com
//
// A license to use the code in this file for the OpenDCL project has been granted
// by Owen Wengerd and ManuSoft. This code is provided under the terms of the GPL
// license governing OpenDCL. This code may not be used for any other purpose
// without the express written consent of Owen Wengerd or ManuSoft.


// For convenience, ensure it's defined here in the header for all to use
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#ifndef WM_DPICHANGED_BEFOREPARENT
#define WM_DPICHANGED_AFTERPARENT 0x02E2
#endif

#ifndef WM_DPICHANGED_AFTERPARENT
#define WM_DPICHANGED_AFTERPARENT 0x02E3
#endif

#ifndef WM_GETDPISCALEDSIZE
#define WM_GETDPISCALEDSIZE 0x02E4
#endif

#ifndef DPI_AWARENESS_CONTEXT_UNAWARE
#define DPI_AWARENESS_CONTEXT_UNAWARE              ((DPI_AWARENESS_CONTEXT)-1)
#endif

#ifndef DPI_AWARENESS_CONTEXT_SYSTEM_AWARE
#define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE         ((DPI_AWARENESS_CONTEXT)-2)
#endif

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE    ((DPI_AWARENESS_CONTEXT)-3)
#endif

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
#endif

#if !defined(HMONITOR_DECLARED) && (WINVER < 0x0500)
DECLARE_HANDLE(HMONITOR);
#define HMONITOR_DECLARED
#endif

#ifndef MONITOR_DEFAULTTOPRIMARY
#define MONITOR_DEFAULTTONULL       0x00000000
#define MONITOR_DEFAULTTOPRIMARY    0x00000001
#define MONITOR_DEFAULTTONEAREST    0x00000002
#endif

__if_not_exists(_DPI_AWARENESS)
{
typedef enum _DPI_AWARENESS { 
	DPI_AWARENESS_INVALID            = -1,
	DPI_AWARENESS_UNAWARE            = 0,
	DPI_AWARENESS_SYSTEM_AWARE       = 1,
	DPI_AWARENESS_PER_MONITOR_AWARE  = 2
} DPI_AWARENESS;
}

__if_not_exists(_PROCESS_DPI_AWARENESS)
{
typedef enum _PROCESS_DPI_AWARENESS { 
	PROCESS_DPI_UNAWARE            = 0,
	PROCESS_SYSTEM_DPI_AWARE       = 1,
	PROCESS_PER_MONITOR_DPI_AWARE  = 2
} PROCESS_DPI_AWARENESS;
}

__if_not_exists(DPI_AWARENESS_CONTEXT)
{
DECLARE_HANDLE(DPI_AWARENESS_CONTEXT);
}

__if_not_exists(_MONITOR_DPI_TYPE)
{
typedef enum _MONITOR_DPI_TYPE { 
	MDT_EFFECTIVE_DPI  = 0,
	MDT_ANGULAR_DPI    = 1,
	MDT_RAW_DPI        = 2,
	MDT_DEFAULT        = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
}

__if_not_exists(_DIALOG_DPI_CHANGE_BEHAVIORS)
{
typedef enum _DIALOG_DPI_CHANGE_BEHAVIORS { 
	DDC_DEFAULT                   = 0x0000,
	DDC_DISABLE_ALL               = 0x0001,
	DDC_DISABLE_RESIZE            = 0x0002,
	DDC_DISABLE_CONTROL_RELAYOUT  = 0x0003
} DIALOG_DPI_CHANGE_BEHAVIORS;
}

__if_not_exists(_DIALOG_SCALING_BEHAVIOR)
{
typedef enum _DIALOG_SCALING_BEHAVIOR { 
	DCDC_DEFAULT              = 0x0000,
	DCDC_DISABLE_FONT_UPDATE  = 0x0001,
	DCDC_DISABLE_RELAYOUT     = 0x0002
} DIALOG_SCALING_BEHAVIOR;
}


class DpiAwarenessHelper
{
public:
	static HMONITOR MonitorFromPoint( POINT pt, DWORD dwFlags );
	static HRESULT GetDpiForMonitor( HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY );
	static UINT GetDpiForSystem();
	static UINT GetDpiForWindow( HWND hwnd );
	static HRESULT GetProcessDpiAwareness( HANDLE hprocess, PROCESS_DPI_AWARENESS* value );
	static int GetSystemMetricsForDpi( int nIndex, UINT dpi );
	static DPI_AWARENESS_CONTEXT GetThreadDpiAwarenessContext();
	static DPI_AWARENESS_CONTEXT GetWindowDpiAwarenessContext( HWND hwnd );
	static BOOL IsValidDpiAwarenessContext( DPI_AWARENESS_CONTEXT value );
	static BOOL LogicalToPhysicalPoint( HWND hwnd, LPPOINT lpPoint );
	static BOOL LogicalToPhysicalPointForPerMonitorDPI( HWND hwnd, LPPOINT lpPoint );
	static BOOL PhysicalToLogicalPoint( HWND hwnd, LPPOINT lpPoint );
	static BOOL PhysicalToLogicalPointForPerMonitorDPI( HWND hwnd, LPPOINT lpPoint );
	static DPI_AWARENESS GetAwarenessFromDpiAwarenessContext( DPI_AWARENESS_CONTEXT value );
	static BOOL SystemParametersInfoForDpi( UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi );
	static HTHEME OpenThemeDataForDpi( HWND hwnd, PCWSTR pszClassIdList, UINT dpi );
	static BOOL AdjustWindowRectExForDpi( LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi );
	static BOOL SetDialogDpiChangeBehavior( HWND hDlg, DIALOG_DPI_CHANGE_BEHAVIORS mask, DIALOG_DPI_CHANGE_BEHAVIORS values );
	static DIALOG_DPI_CHANGE_BEHAVIORS GetDialogDpiChangeBehavior( HWND hDlg );
	static BOOL AreDpiAwarenessContextsEqual( DPI_AWARENESS_CONTEXT dpiContextA, DPI_AWARENESS_CONTEXT dpiContextB );
	static DPI_AWARENESS_CONTEXT SetThreadDpiAwarenessContext( DPI_AWARENESS_CONTEXT dpiContext );
	static BOOL SetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT dpiContext );
	static HRESULT SetProcessDpiAwareness( PROCESS_DPI_AWARENESS value );
};

class DpiAwareness : public DpiAwarenessHelper
{
public:
	DpiAwareness() {}
	virtual ~DpiAwareness() {}
protected:
	DpiAwareness( const DpiAwareness& rhs ); //copy guard
	DpiAwareness& operator=( const DpiAwareness& rhs ); //copy guard

public:
	bool FromDIP( LONG* values, USHORT count ) const;
	bool ToDIP( LONG* values, USHORT count ) const;

protected:
	virtual UINT GetDpi() const { return 96; }

protected:
	static bool IsProcessDpiAware( HANDLE hprocess );
	static bool IsDpiAwarePerMonitorV2( HWND hwnd );
};
