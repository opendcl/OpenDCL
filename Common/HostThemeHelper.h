// HostThemeHelper.h : shared host light/dark UxTheme walk
//

#pragma once

// COLORTHEME (host light/dark) exists on AutoCAD 2015+ and BricsCAD V19+.
// Pre-themed Acad (and matching old BRX) must not compile this class.
#if !((defined(_ARXTARGET) && (_ARXTARGET < 20)) || (defined(_BRXTARGET) && (_BRXTARGET < 19)))
#define ODCL_HOST_COLORTHEME 1
#endif

class CHeaderCtrl;
struct tagNMHDR;


/////////////////////////////////////////////////////////////////////////////
// CHostThemeHelper

#if defined(ODCL_HOST_COLORTHEME)

class CHostThemeHelper
{
public:
	static bool HostMaps();
	static LPCWSTR ThemeClass( bool bVisual );
	static void Apply( HWND hwnd, LPCWSTR pszTheme );
	static void ApplyTree( HWND hwndRoot, LPCWSTR pszTheme );
	static void ApplyHeader( CHeaderCtrl* pHeader );
	static bool PaintHeaderCustomDraw( CHeaderCtrl* pHeader, tagNMHDR* pNMHDR, LRESULT* pResult );
	static void PaintComboDropButton( HDC hdc, const RECT& rc, bool bEnabled );
	static void PaintComboChrome( HWND hwnd );
	static COLORREF DisabledTextColor();
	static COLORREF SoftGlyphColor();
	static COLORREF GridLineColor( COLORREF crBackground );
	static LPCWSTR ScrollTheme();
	static void PaintEtchedRect( HDC hdc, const RECT& rc );
	static void PaintRaisedInner( HDC hdc, const RECT& rc );
	static void InstallNcBorder( HWND hwnd );
	static void InstallNcBorderTree( HWND hwndRoot );
	static bool PaintNcBorder( HWND hwnd );
	static void DrawSmoothEllipse( HDC hdc, const RECT& rc, COLORREF crFill, COLORREF crPen, bool bFill );
};

#else

class CHostThemeHelper
{
public:
	static bool HostMaps() { return false; }
	static LPCWSTR ThemeClass( bool ) { return NULL; }
	static void Apply( HWND, LPCWSTR ) {}
	static void ApplyTree( HWND, LPCWSTR ) {}
	static void ApplyHeader( CHeaderCtrl* ) {}
	static bool PaintHeaderCustomDraw( CHeaderCtrl*, tagNMHDR*, LRESULT* pResult )
	{
		if( pResult )
			*pResult = 0;
		return false;
	}
	static void PaintComboDropButton( HDC, const RECT&, bool ) {}
	static void PaintComboChrome( HWND ) {}
	static COLORREF DisabledTextColor() { return ::GetSysColor( COLOR_GRAYTEXT ); }
	static COLORREF SoftGlyphColor() { return ::GetSysColor( COLOR_BTNTEXT ); }
	static COLORREF GridLineColor( COLORREF ) { return ::GetSysColor( COLOR_BTNFACE ); }
	static LPCWSTR ScrollTheme() { return NULL; }
	static void PaintEtchedRect( HDC, const RECT& ) {}
	static void PaintRaisedInner( HDC, const RECT& ) {}
	static void InstallNcBorder( HWND ) {}
	static void InstallNcBorderTree( HWND ) {}
	static bool PaintNcBorder( HWND ) { return false; }
	static void DrawSmoothEllipse( HDC, const RECT&, COLORREF, COLORREF, bool ) {}
};

#endif

#if defined(ODCL_HOST_COLORTHEME)
inline void CHostThemeHelper::DrawSmoothEllipse( HDC hdc, const RECT& rc, COLORREF crFill, COLORREF crPen, bool bFill )
{
	if( !hdc )
		return;
	const int nW = rc.right - rc.left;
	const int nH = rc.bottom - rc.top;
	if( nW < 3 || nH < 3 )
		return;
	// Build AA on a solid fill - never StretchBlt-sample the destination (that
	// re-samples the previous ring on hover and makes glyphs jitter).
	const COLORREF crBk = bFill ? crFill : ::GetPixel( hdc, rc.left + nW / 2, rc.top + nH / 2 );
	const int nXW = nW * 2;
	const int nXH = nH * 2;
	HDC hdcMem = ::CreateCompatibleDC( hdc );
	if( !hdcMem )
		return;
	HBITMAP hbm = ::CreateCompatibleBitmap( hdc, nXW, nXH );
	if( !hbm )
	{
		::DeleteDC( hdcMem );
		return;
	}
	HGDIOBJ hbmOld = ::SelectObject( hdcMem, hbm );
	HBRUSH brBk = ::CreateSolidBrush( crBk );
	RECT rcMem = { 0, 0, nXW, nXH };
	::FillRect( hdcMem, &rcMem, brBk );
	::DeleteObject( brBk );
	HPEN pen = ::CreatePen( PS_SOLID, 2, crPen );
	HBRUSH br = bFill ? ::CreateSolidBrush( crFill ) : (HBRUSH)::GetStockObject( NULL_BRUSH );
	HGDIOBJ penOld = ::SelectObject( hdcMem, pen );
	HGDIOBJ brOld = ::SelectObject( hdcMem, br );
	::Ellipse( hdcMem, 1, 1, nXW - 1, nXH - 1 );
	::SelectObject( hdcMem, brOld );
	::SelectObject( hdcMem, penOld );
	if( bFill )
		::DeleteObject( br );
	::DeleteObject( pen );
	const int nOldMode = ::SetStretchBltMode( hdc, HALFTONE );
	::StretchBlt( hdc, rc.left, rc.top, nW, nH, hdcMem, 0, 0, nXW, nXH, SRCCOPY );
	::SetStretchBltMode( hdc, nOldMode );
	::SelectObject( hdcMem, hbmOld );
	::DeleteObject( hbm );
	::DeleteDC( hdcMem );
}
#endif

