#pragma once

// These two classes expose and abstract UxTheme API functions without an implicit reference to 
// UxTheme.dll. When API functions are called, the calls are passed through to the real functions 
// if they are available; else they return E_NOTIMPL or default value (e.g. when running on an 
// earlier version of Windows).
//
// Copyright 2013 - 2014 ManuSoft. All Rights Reserved.
// http://www.manusoft.com
//
// A license to use the code in this file for the OpenDCL project has been granted
// by Owen Wengerd and ManuSoft. This code is provided under the terms of the GPL
// license governing OpenDCL. This code may not be used for any other purpose
// without the express written consent of Owen Wengerd or ManuSoft.


#if (_MSC_VER >= 1500) //VS 2008+
#include <uxtheme.h>
#include <vsstyle.h>
#elif (_MSC_VER == 1400) //VS 2005
#include <uxtheme.h>
#include <tmschema.h>
#endif


// For convenience, ensure it's defined here in the header for all to use
#ifndef NM_THEMECHANGED
#define NM_THEMECHANGED (NM_FIRST-22)
#endif

#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED 0x031A
#endif

__if_not_exists(HTHEME)
{
typedef HANDLE HTHEME;
}
__if_not_exists(HPAINTBUFFER)
{
typedef HANDLE HPAINTBUFFER;
}
__if_not_exists(HANIMATIONBUFFER)
{
typedef HANDLE HANIMATIONBUFFER;
}
__if_not_exists(DTBGOPTS)
{
typedef struct _DTBGOPTS DTBGOPTS, *PDTBGOPTS;
}
__if_not_exists(MARGINS)
{
typedef struct _MARGINS MARGINS, *PMARGINS;
}
__if_not_exists(DTTOPTS)
{
typedef struct _DTTOPTS DTTOPTS, *PDTTOPTS;
}


class UxTheme
{
	HTHEME mhTheme;
	bool mbMustCloseTheme;

public:
	UxTheme();
	UxTheme( HWND hWnd, LPCWSTR pszClassList );
	UxTheme( HWND hWnd, LPCWSTR pszClassList, DWORD dwFlags );
	UxTheme( HTHEME hTheme, bool bAutoClose = false );
	~UxTheme();
protected:
	UxTheme( UxTheme& rhs ); //copy guard
	UxTheme& operator=( UxTheme& rhs ); //copy guard

public:
	operator HTHEME() const { return mhTheme; }
	HTHEME Detach() { HTHEME hOld = mhTheme; mhTheme = NULL; mbMustCloseTheme = false; return hOld; }
	HTHEME Attach( HTHEME hTheme, bool bAutoClose = false ) { if( mbMustCloseTheme ) Close(); HTHEME hOld = mhTheme; mhTheme = hTheme; mbMustCloseTheme = bAutoClose; return hOld; }
	void Close() { if( mbMustCloseTheme && mhTheme ) CloseThemeData( mhTheme ); mhTheme = NULL; mbMustCloseTheme = false; }

	// Theme API functions (from UxTheme.h)
	static BOOL WINAPI BeginPanningFeedback( HWND hwnd );
	static BOOL WINAPI UpdatePanningFeedback( HWND hwnd, LONG lTotalOverpanOffsetX, LONG lTotalOverpanOffsetY, BOOL fInInertia );
	static BOOL WINAPI EndPanningFeedback( HWND hwnd, BOOL fAnimateBack );
	HRESULT DrawThemeBackground( HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect ) const;
	HRESULT DrawThemeBackgroundEx( HDC hdc, int iPartId, int iStateId, LPCRECT pRect, const DTBGOPTS *pOptions ) const;
	HRESULT DrawThemeText( HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect ) const;
	HRESULT GetThemeBackgroundContentRect( HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect ) const;
	HRESULT GetThemeBackgroundExtent( HDC hdc, int iPartId, int iStateId, LPCRECT pContentRect, LPRECT pExtentRect ) const;
	HRESULT GetThemeBackgroundRegion( HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion ) const;
	HRESULT GetThemePartSize( HDC hdc, int iPartId, int iStateId, LPCRECT prc, enum THEMESIZE eSize, SIZE *psz ) const;
	HRESULT GetThemeTextExtent( HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchCharCount, DWORD dwTextFlags, LPCRECT pBoundingRect, LPRECT pExtentRect ) const;
	HRESULT GetThemeTextMetrics( HDC hdc, int iPartId, int iStateId, TEXTMETRICW *ptm ) const;
	HRESULT HitTestThemeBackground( HDC hdc, int iPartId, int iStateId, DWORD dwOptions, LPCRECT pRect, HRGN hrgn, POINT ptTest, WORD *pwHitTestCode ) const;
	HRESULT DrawThemeEdge( HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect ) const;
	HRESULT DrawThemeIcon( HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HIMAGELIST himl, int iImageIndex ) const;
	BOOL IsThemePartDefined( int iPartId, int iStateId ) const;
	BOOL IsThemeBackgroundPartiallyTransparent( int iPartId, int iStateId ) const;
	HRESULT GetThemeColor( int iPartId, int iStateId, int iPropId, COLORREF *pColor ) const;
	HRESULT GetThemeMetric( HDC hdc, int iPartId, int iStateId, int iPropId, int *piVal ) const;
	HRESULT GetThemeString( int iPartId, int iStateId, int iPropId, LPWSTR pszBuff, int cchMaxBuffChars ) const;
	HRESULT GetThemeBool( int iPartId, int iStateId, int iPropId, BOOL *pfVal ) const;
	HRESULT GetThemeInt( int iPartId, int iStateId, int iPropId, int *piVal ) const;
	HRESULT GetThemeEnumValue( int iPartId, int iStateId, int iPropId, int *piVal ) const;
	HRESULT GetThemePosition( int iPartId, int iStateId, int iPropId, POINT *pPoint ) const;
	HRESULT GetThemeFont( HDC hdc, int iPartId, int iStateId, int iPropId, LOGFONTW *pFont ) const;
	HRESULT GetThemeRect( int iPartId, int iStateId, int iPropId, LPRECT pRect ) const;
	HRESULT GetThemeMargins( HDC hdc, int iPartId, int iStateId, int iPropId, LPCRECT prc, MARGINS *pMargins ) const;
	HRESULT GetThemeIntList( int iPartId, int iStateId, int iPropId, struct INTLIST *pIntList ) const;
	HRESULT GetThemePropertyOrigin( int iPartId, int iStateId, int iPropId, enum PROPERTYORIGIN *pOrigin ) const;
	static HRESULT SetWindowTheme( HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList );
	HRESULT GetThemeFilename( int iPartId, int iStateId, int iPropId, LPWSTR pszThemeFileName, int cchMaxBuffChars ) const;
	COLORREF GetThemeSysColor( int iColorId ) const;
	HBRUSH GetThemeSysColorBrush( int iColorId ) const;
	BOOL GetThemeSysBool( int iBoolId ) const;
	int GetThemeSysSize( int iSizeId ) const;
	HRESULT GetThemeSysFont( int iFontId, LOGFONTW *plf ) const;
	HRESULT GetThemeSysString( int iStringId, LPWSTR pszStringBuff, int cchMaxStringChars ) const;
	HRESULT GetThemeSysInt( int iIntId, int *piValue ) const;
	static BOOL IsThemeActive( VOID );
	static BOOL IsAppThemed( VOID );
	static HTHEME GetWindowTheme( HWND hwnd );
	static HRESULT EnableThemeDialogTexture( HWND hwnd, DWORD dwFlags );
	static BOOL IsThemeDialogTextureEnabled( HWND hwnd );
	static DWORD GetThemeAppProperties( VOID );
	static void SetThemeAppProperties( DWORD dwFlags );
	static HRESULT GetCurrentThemeName( LPWSTR pszThemeFileName, int cchMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR pszSizeBuff, int cchMaxSizeChars );
	static HRESULT GetThemeDocumentationProperty( LPCWSTR pszThemeName, LPCWSTR pszPropertyName, LPWSTR pszValueBuff, int cchMaxValChars );
	static HRESULT DrawThemeParentBackground( HWND hwnd, HDC hdc, const RECT* prc );
	static HRESULT EnableTheming( BOOL fEnable );
	static HRESULT DrawThemeParentBackgroundEx( HWND hwnd, HDC hdc, DWORD dwFlags, const RECT* prc );
	static HRESULT SetWindowThemeAttribute( HWND hwnd, enum WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute );
	static HRESULT SetWindowThemeNonClientAttributes( HWND hwnd, DWORD dwMask, DWORD dwAttributes );
	HRESULT DrawThemeTextEx( HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS *pOptions ) const;
	HRESULT GetThemeBitmap( int iPartId, int iStateId, int iPropId, ULONG dwFlags, HBITMAP* phBitmap ) const;
	HRESULT GetThemeStream( int iPartId, int iStateId, int iPropId, VOID **ppvStream, DWORD *pcbStream, HINSTANCE hInst ) const;
	static HRESULT BufferedPaintInit( VOID );
	static HRESULT BufferedPaintUnInit( VOID );
	static HPAINTBUFFER BeginBufferedPaint( HDC hdcTarget, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, HDC *phdc );
	static HRESULT EndBufferedPaint( HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget );
	static HRESULT GetBufferedPaintTargetRect( HPAINTBUFFER hBufferedPaint, RECT *prc );
	static HDC GetBufferedPaintTargetDC( HPAINTBUFFER hBufferedPaint );
	static HDC GetBufferedPaintDC( HPAINTBUFFER hBufferedPaint );
	static HRESULT GetBufferedPaintBits( HPAINTBUFFER hBufferedPaint, RGBQUAD **ppbBuffer, int *pcxRow );
	static HRESULT BufferedPaintClear( HPAINTBUFFER hBufferedPaint, const RECT *prc );
	static HRESULT BufferedPaintSetAlpha( HPAINTBUFFER hBufferedPaint, const RECT *prc, BYTE alpha );
	static HRESULT BufferedPaintStopAllAnimations( HWND hwnd );
	static HANIMATIONBUFFER BeginBufferedAnimation( HWND hwnd, HDC hdcTarget, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, struct BP_ANIMATIONPARAMS *pAnimationParams, HDC *phdcFrom, HDC *phdcTo );
	static HRESULT EndBufferedAnimation( HANIMATIONBUFFER hbpAnimation, BOOL fUpdateTarget );
	static BOOL BufferedPaintRenderAnimation( HWND hwnd, HDC hdcTarget );
	static BOOL IsCompositionActive( VOID );
	HRESULT GetThemeTransitionDuration( int iPartId, int iStateIdFrom, int iStateIdTo, int iPropId, DWORD *pdwDuration ) const;

	// Left exposed for completeness, but client code should rarely call these directly
	static HTHEME OpenThemeData( HWND hwnd, LPCWSTR pszClassList );
	static HTHEME OpenThemeDataEx( HWND hwnd, LPCWSTR pszClassList, DWORD dwFlags );
	static HRESULT CloseThemeData( HTHEME hTheme );
};


class WndTheme : public UxTheme
{
	HWND mhWnd;

public:
	WndTheme() : mhWnd( NULL ) {}
	WndTheme( HWND hWnd, LPCWSTR pszClassList ) : UxTheme( hWnd, pszClassList ), mhWnd( hWnd ) {}
	WndTheme( HWND hWnd, LPCWSTR pszClassList, DWORD dwFlags ) : UxTheme( hWnd, pszClassList, dwFlags ), mhWnd( hWnd ) {}
	WndTheme( HTHEME hTheme, HWND hWnd, bool bAutoClose = false ) : UxTheme( hTheme, bAutoClose ), mhWnd( hWnd ) {}
	~WndTheme() {}
protected:
	WndTheme( WndTheme& rhs ); //copy guard
	WndTheme& operator=( WndTheme& rhs ); //copy guard

public:
	HWND HWnd() const { return mhWnd; }
	HTHEME Attach( HTHEME hTheme, HWND hwnd, bool bAutoClose = false ) { mhWnd = hwnd; return __super::Attach( hTheme, bAutoClose ); }
	void Close() { __super::Close(); mhWnd = NULL; }

	// Theme API functions (from UxTheme.h) -- only overriding the ones that need the stored HWND
	BOOL WINAPI BeginPanningFeedback() const;
	BOOL WINAPI UpdatePanningFeedback( LONG lTotalOverpanOffsetX, LONG lTotalOverpanOffsetY, BOOL fInInertia ) const;
	BOOL WINAPI EndPanningFeedback( BOOL fAnimateBack ) const;
	HTHEME GetWindowTheme( VOID ) const;
	HRESULT SetWindowTheme( LPCWSTR pszSubAppName, LPCWSTR pszSubIdList ) const;
	HRESULT EnableThemeDialogTexture( DWORD dwFlags ) const;
	BOOL IsThemeDialogTextureEnabled() const;
	HRESULT DrawThemeParentBackground( HDC hdc, const RECT* prc ) const;
	HRESULT DrawThemeParentBackgroundEx( HDC hdc, DWORD dwFlags, const RECT* prc ) const;
	HRESULT SetWindowThemeAttribute( enum WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute ) const;
	HRESULT SetWindowThemeNonClientAttributes( DWORD dwMask, DWORD dwAttributes ) const;
	HRESULT BufferedPaintStopAllAnimations() const;
	HANIMATIONBUFFER BeginBufferedAnimation( HDC hdcTarget, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, struct BP_ANIMATIONPARAMS *pAnimationParams, HDC *phdcFrom, HDC *phdcTo ) const;
	BOOL BufferedPaintRenderAnimation( HDC hdcTarget ) const;
};


class DcTheme : public WndTheme
{
	HDC mhDC;

public:
	DcTheme() : mhDC( NULL ) {}
	DcTheme( HWND hWnd, LPCWSTR pszClassList, HDC hdc ) : WndTheme( hWnd, pszClassList ), mhDC( hdc ) {}
	DcTheme( HWND hWnd, LPCWSTR pszClassList, DWORD dwFlags, HDC hdc ) : WndTheme( hWnd, pszClassList, dwFlags ), mhDC( hdc ) {}
	DcTheme( HTHEME hTheme, HWND hWnd, HDC hdc, bool bAutoClose = false ) : WndTheme( hTheme, hWnd, bAutoClose ), mhDC( hdc ) {}
	DcTheme( const WndTheme& Wnd, HDC hdc ) : WndTheme( Wnd, Wnd.HWnd(), false ), mhDC( hdc ) {}
	~DcTheme() {}
protected:
	DcTheme( DcTheme& rhs ); //copy guard
	DcTheme& operator=( DcTheme& rhs ); //copy guard

public:
	HDC DC() const { return mhDC; }
	HTHEME Attach( HTHEME hTheme, HWND hWnd, HDC hdc, bool bAutoClose = false ) { mhDC = hdc; return __super::Attach( hTheme, hWnd, bAutoClose ); }
	void Close() { __super::Close(); mhDC = NULL; }

	// Utility
	class AutoDcPaintBuffer BufferedPaint( const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, HDC *phdc );

	// Theme API functions (from UxTheme.h) -- only overriding the ones that need the stored HDC
	HRESULT DrawThemeBackground( int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect ) const;
	HRESULT DrawThemeBackgroundEx( int iPartId, int iStateId, LPCRECT pRect, const DTBGOPTS *pOptions ) const;
	HRESULT DrawThemeText( int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect ) const;
	HRESULT GetThemeBackgroundContentRect( int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect ) const;
	HRESULT GetThemeBackgroundExtent( int iPartId, int iStateId, LPCRECT pContentRect, LPRECT pExtentRect ) const;
	HRESULT GetThemeBackgroundRegion( int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion ) const;
	HRESULT GetThemePartSize( int iPartId, int iStateId, LPCRECT prc, enum THEMESIZE eSize, SIZE *psz ) const;
	HRESULT GetThemeTextExtent( int iPartId, int iStateId, LPCWSTR pszText, int cchCharCount, DWORD dwTextFlags, LPCRECT pBoundingRect, LPRECT pExtentRect ) const;
	HRESULT GetThemeTextMetrics( int iPartId, int iStateId, TEXTMETRICW *ptm ) const;
	HRESULT HitTestThemeBackground( int iPartId, int iStateId, DWORD dwOptions, LPCRECT pRect, HRGN hrgn, POINT ptTest, WORD *pwHitTestCode ) const;
	HRESULT DrawThemeEdge( int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect ) const;
	HRESULT DrawThemeIcon( int iPartId, int iStateId, LPCRECT pRect, HIMAGELIST himl, int iImageIndex ) const;
	HRESULT GetThemeMetric( int iPartId, int iStateId, int iPropId, int *piVal ) const;
	HRESULT GetThemeFont( int iPartId, int iStateId, int iPropId, LOGFONTW *pFont ) const;
	HRESULT GetThemeMargins( int iPartId, int iStateId, int iPropId, LPCRECT prc, MARGINS *pMargins ) const;
	HRESULT DrawThemeParentBackground( const RECT* prc ) const;
	HRESULT DrawThemeParentBackgroundEx( DWORD dwFlags, const RECT* prc ) const;
	HRESULT DrawThemeTextEx( int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS *pOptions ) const;
	HPAINTBUFFER BeginBufferedPaint( const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, HDC *phdc ) const;
	HANIMATIONBUFFER BeginBufferedAnimation( const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, struct BP_ANIMATIONPARAMS *pAnimationParams, HDC *phdcFrom, HDC *phdcTo ) const;
	BOOL BufferedPaintRenderAnimation( VOID ) const;
};


// This utility class manages a paint buffer, and also calls Init/UnInit automatically
class AutoDcPaintBuffer
{
	HPAINTBUFFER mhPaintBuffer;
	class DcTheme& mDC;

	static HPAINTBUFFER InitAndBegin( const DcTheme& DC, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, HDC *phdc )
	{
		UxTheme::BufferedPaintInit();
		return DC.BeginBufferedPaint( prcTarget, dwFormat, pPaintParams, phdc );
	}

public:
	AutoDcPaintBuffer( DcTheme& DC, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, HDC *phdc )
		: mhPaintBuffer( InitAndBegin( DC, prcTarget, dwFormat, pPaintParams, phdc ) ), mDC( DC ) {}
	~AutoDcPaintBuffer() { if( mhPaintBuffer ) mDC.EndBufferedPaint( mhPaintBuffer, TRUE ); UxTheme::BufferedPaintUnInit(); }

	friend class DcTheme;
	AutoDcPaintBuffer( AutoDcPaintBuffer& rhs ) : mhPaintBuffer( rhs.mhPaintBuffer ), mDC( rhs.mDC ) { rhs.Cancel(); }
protected:
	AutoDcPaintBuffer& operator=( AutoDcPaintBuffer& rhs ); //copy guard

public:
	operator HPAINTBUFFER () const { return mhPaintBuffer; }
	void Cancel() { if( mhPaintBuffer ) mDC.EndBufferedPaint( mhPaintBuffer, FALSE ); mhPaintBuffer = NULL; }
};


class PaintBufferTheme : public WndTheme
{
	HPAINTBUFFER mhPaintBuffer;

public:
	PaintBufferTheme() : mhPaintBuffer( NULL ) {}
	PaintBufferTheme( HWND hWnd, LPCWSTR pszClassList, HPAINTBUFFER hPaintBuffer ) : WndTheme( hWnd, pszClassList ), mhPaintBuffer( hPaintBuffer ) {}
	PaintBufferTheme( HWND hWnd, LPCWSTR pszClassList, DWORD dwFlags, HPAINTBUFFER hPaintBuffer ) : WndTheme( hWnd, pszClassList, dwFlags ), mhPaintBuffer( hPaintBuffer ) {}
	PaintBufferTheme( HTHEME hTheme, HWND hWnd, HPAINTBUFFER hPaintBuffer, bool bAutoClose = false ) : WndTheme( hTheme, hWnd, bAutoClose ), mhPaintBuffer( hPaintBuffer ) {}
	PaintBufferTheme( const WndTheme& Wnd, HPAINTBUFFER hPaintBuffer ) : WndTheme( Wnd, Wnd.HWnd(), false ), mhPaintBuffer( hPaintBuffer ) {}
	~PaintBufferTheme() {}
protected:
	PaintBufferTheme( PaintBufferTheme& rhs ); //copy guard
	PaintBufferTheme& operator=( PaintBufferTheme& rhs ); //copy guard

public:
	HPAINTBUFFER PaintBuffer() const { return mhPaintBuffer; }
	HTHEME Attach( HTHEME hTheme, HWND hWnd, HPAINTBUFFER hPaintBuffer, bool bAutoClose = false ) { mhPaintBuffer = hPaintBuffer; return __super::Attach( hTheme, hWnd, bAutoClose ); }
	void Close() { __super::Close(); mhPaintBuffer = NULL; }

	// Theme API functions (from UxTheme.h) -- only overriding the ones that need the stored HPAINTBUFFER
	HRESULT EndBufferedPaint( BOOL fUpdateTarget ) const;
	HRESULT GetBufferedPaintTargetRect( RECT *prc ) const;
	HDC GetBufferedPaintTargetDC() const;
	HDC GetBufferedPaintDC() const;
	HRESULT GetBufferedPaintBits( RGBQUAD **ppbBuffer, int *pcxRow ) const;
	HRESULT BufferedPaintClear( const RECT *prc ) const;
	HRESULT BufferedPaintSetAlpha( const RECT *prc, BYTE alpha ) const;
};


inline AutoDcPaintBuffer DcTheme::BufferedPaint( const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, HDC *phdc )
{
	return AutoDcPaintBuffer( *this, prcTarget, dwFormat, pPaintParams, phdc );
}
