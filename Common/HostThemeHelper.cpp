#include "stdafx.h"
#include "HostThemeHelper.h"

#if defined(ODCL_HOST_COLORTHEME)

#include "ThemeAPI.h"
#include "ColorService.h"

#include <commctrl.h>


bool CHostThemeHelper::HostMaps()
{
	return OdclSysColor( COLOR_WINDOW ) != ::GetSysColor( COLOR_WINDOW );
}

LPCWSTR CHostThemeHelper::ThemeClass( bool bVisual )
{
	return (bVisual && !HostMaps())? NULL : L"";
}

void CHostThemeHelper::Apply( HWND hwnd, LPCWSTR pszTheme )
{
	if( hwnd )
		UxTheme::SetWindowTheme( hwnd, pszTheme, pszTheme );
}

void CHostThemeHelper::ApplyTree( HWND hwndRoot, LPCWSTR pszTheme )
{
	if( !hwndRoot )
		return;
	Apply( hwndRoot, pszTheme );
	for( HWND hwndChild = ::FindWindowEx( hwndRoot, NULL, NULL, NULL );
			 hwndChild;
			 hwndChild = ::FindWindowEx( hwndRoot, hwndChild, NULL, NULL ) )
	{
		ApplyTree( hwndChild, pszTheme );
		::InvalidateRect( hwndChild, NULL, TRUE );
	}
}

void CHostThemeHelper::ApplyHeader( CHeaderCtrl* pHeader )
{
	if( pHeader && pHeader->m_hWnd )
		Apply( pHeader->m_hWnd, HostMaps()? L"" : NULL );
}

bool CHostThemeHelper::PaintHeaderCustomDraw( CHeaderCtrl* pHeader, tagNMHDR* pNMHDR, LRESULT* pResult )
{
	if( !pResult )
		return false;
	*pResult = CDRF_DODEFAULT;
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast< LPNMCUSTOMDRAW >( pNMHDR );
	if( !pHeader || !pNMCD || !HostMaps() )
		return false;
	if( pNMCD->hdr.hwndFrom != pHeader->m_hWnd )
		return false;
	switch( pNMCD->dwDrawStage )
	{
	case CDDS_PREPAINT:
		{
			CDC* pDC = CDC::FromHandle( pNMCD->hdc );
			if( pDC )
			{
				CRect rc;
				pHeader->GetClientRect( &rc );
				pDC->FillSolidRect( &rc, OdclSysColor( COLOR_BTNFACE ) );
			}
			*pResult = CDRF_NOTIFYITEMDRAW;
		}
		return true;
	case CDDS_ITEMPREPAINT:
		{
			CDC* pDC = CDC::FromHandle( pNMCD->hdc );
			if( !pDC )
				return true;
			CRect rc( pNMCD->rc );
			pDC->FillSolidRect( &rc, OdclSysColor( COLOR_BTNFACE ) );
			HDITEM item = {};
			TCHAR sz[256] = {};
			item.mask = HDI_TEXT;
			item.pszText = sz;
			item.cchTextMax = 255;
			Header_GetItem( pHeader->m_hWnd, (int)pNMCD->dwItemSpec, &item );
			const COLORREF crOld = pDC->SetTextColor( OdclSysColor( COLOR_BTNTEXT ) );
			const int nOldBk = pDC->SetBkMode( TRANSPARENT );
			CFont* pFont = pHeader->GetFont();
			CFont* pOldFont = pFont ? pDC->SelectObject( pFont ) : NULL;
			rc.DeflateRect( 6, 0 );
			pDC->DrawText( sz, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );
			if( pOldFont )
				pDC->SelectObject( pOldFont );
			pDC->SetBkMode( nOldBk );
			pDC->SetTextColor( crOld );
			*pResult = CDRF_SKIPDEFAULT;
		}
		return true;
	}
	return true;
}

void CHostThemeHelper::PaintComboDropButton( HDC hdc, const RECT& rc, bool bEnabled )
{
	if( !hdc )
		return;
	CRect rcBtn( rc );
	if( rcBtn.Width() <= 0 || rcBtn.Height() <= 0 )
		return;
	CDC* pDC = CDC::FromHandle( hdc );
	if( !pDC )
		return;
	pDC->FillSolidRect( &rcBtn, OdclSysColor( COLOR_BTNFACE ) );
	CPen pen( PS_SOLID, 1, OdclSysColor( COLOR_3DSHADOW ) );
	CPen* pOldPen = pDC->SelectObject( &pen );
	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( NULL_BRUSH );
	pDC->Rectangle( rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom );
	pDC->SelectObject( pOldBrush );
	pDC->SelectObject( pOldPen );

	const int cx = (rcBtn.left + rcBtn.right) / 2;
	const int cy = (rcBtn.top + rcBtn.bottom) / 2;
	const int s = max( 2, min( rcBtn.Width(), rcBtn.Height() ) / 4 );
	POINT pts[3] = {
		{ cx - s, cy - s / 2 },
		{ cx + s, cy - s / 2 },
		{ cx, cy + s / 2 }
	};
	CBrush br( bEnabled ? SoftGlyphColor() : DisabledTextColor() );
	pOldBrush = pDC->SelectObject( &br );
	pOldPen = (CPen*)pDC->SelectStockObject( NULL_PEN );
	pDC->Polygon( pts, 3 );
	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldBrush );
}

COLORREF CHostThemeHelper::DisabledTextColor()
{
	const COLORREF crFg = OdclSysColor( COLOR_BTNTEXT );
	const COLORREF crBg = OdclSysColor( COLOR_BTNFACE );
	// Same polarity as enabled text so ClearType is not inverted. COLOR_GRAYTEXT
	// is a mid-gray that fringes on dark CAD chrome.
	return RGB(
		( GetRValue( crFg ) * 2 + GetRValue( crBg ) * 3 ) / 5,
		( GetGValue( crFg ) * 2 + GetGValue( crBg ) * 3 ) / 5,
		( GetBValue( crFg ) * 2 + GetBValue( crBg ) * 3 ) / 5 );
}

COLORREF CHostThemeHelper::SoftGlyphColor()
{
	const COLORREF crFg = OdclSysColor( COLOR_BTNTEXT );
	const COLORREF crBg = OdclSysColor( COLOR_BTNFACE );
	return RGB(
		( GetRValue( crFg ) + GetRValue( crBg ) * 2 ) / 3,
		( GetGValue( crFg ) + GetGValue( crBg ) * 2 ) / 3,
		( GetBValue( crFg ) + GetBValue( crBg ) * 2 ) / 3 );
}

COLORREF CHostThemeHelper::GridLineColor( COLORREF crBackground )
{
	// BricsCAD Properties panel (COLORTHEME 0): row (45,49,53), line (68,72,77).
	return RGB(
		min( 255, GetRValue( crBackground ) + 23 ),
		min( 255, GetGValue( crBackground ) + 23 ),
		min( 255, GetBValue( crBackground ) + 24 ) );
}

void CHostThemeHelper::PaintComboChrome( HWND hwnd )
{
	if( !hwnd || !HostMaps() )
		return;
	COMBOBOXINFO cbi = {};
	cbi.cbSize = sizeof( cbi );
	HWND hwndCombo = hwnd;
	if( !::GetComboBoxInfo( hwnd, &cbi ) )
	{
		HWND hwndInner = ::FindWindowEx( hwnd, NULL, _T("ComboBox"), NULL );
		if( !hwndInner || !::GetComboBoxInfo( hwndInner, &cbi ) )
			return;
		hwndCombo = hwndInner;
	}
	HDC hdc = ::GetDC( hwndCombo );
	if( hdc )
	{
		RECT rc = {};
		::GetClientRect( hwndCombo, &rc );
		PaintEtchedRect( hdc, rc );
		::InflateRect( &rc, -1, -1 );
		PaintEtchedRect( hdc, rc );
		PaintComboDropButton( hdc, cbi.rcButton, ::IsWindowEnabled( hwndCombo ) != FALSE );
		::ReleaseDC( hwndCombo, hdc );
	}
	InstallNcBorder( hwndCombo );
	PaintNcBorder( hwndCombo );
	if( hwndCombo != hwnd )
	{
		InstallNcBorder( hwnd );
		PaintNcBorder( hwnd );
		HDC hdcEx = ::GetDC( hwnd );
		if( hdcEx )
		{
			RECT rcEx = {};
			::GetClientRect( hwnd, &rcEx );
			PaintEtchedRect( hdcEx, rcEx );
			::ReleaseDC( hwnd, hdcEx );
		}
	}
}

LPCWSTR CHostThemeHelper::ScrollTheme()
{
	return HostMaps()? L"DarkMode_Explorer" : NULL;
}

namespace {

static LPCTSTR kNcBorderOldProcProp = _T("OdclNcBr");

void FillEdgeBand( HDC hdc, int x, int y, int cx, int cy, COLORREF cr )
{
	RECT rc = { x, y, x + cx, y + cy };
	const COLORREF crOld = ::SetBkColor( hdc, cr );
	::ExtTextOut( hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL );
	::SetBkColor( hdc, crOld );
}

void FrameRectColor( HDC hdc, const RECT& rc, COLORREF cr )
{
	const int w = rc.right - rc.left;
	const int h = rc.bottom - rc.top;
	if( w <= 0 || h <= 0 )
		return;
	FillEdgeBand( hdc, rc.left, rc.top, w, 1, cr );
	FillEdgeBand( hdc, rc.left, rc.bottom - 1, w, 1, cr );
	FillEdgeBand( hdc, rc.left, rc.top, 1, h, cr );
	FillEdgeBand( hdc, rc.right - 1, rc.top, 1, h, cr );
}

void DrawNcScrollArrow( HDC hdc, const RECT& rc, int nDir )
{
	FillEdgeBand( hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
		OdclSysColor( COLOR_BTNFACE ) );
	FrameRectColor( hdc, rc, OdclSysColor( COLOR_3DSHADOW ) );
	CDC* pDC = CDC::FromHandle( hdc );
	if( !pDC )
		return;
	const int cx = (rc.left + rc.right) / 2;
	const int cy = (rc.top + rc.bottom) / 2;
	const int s = max( 2, min( rc.right - rc.left, rc.bottom - rc.top ) / 4 );
	POINT pts[3] = {};
	switch( nDir )
	{
	case 0:
		pts[0].x = cx; pts[0].y = cy - s;
		pts[1].x = cx - s; pts[1].y = cy + s / 2;
		pts[2].x = cx + s; pts[2].y = cy + s / 2;
		break;
	case 1:
		pts[0].x = cx; pts[0].y = cy + s;
		pts[1].x = cx - s; pts[1].y = cy - s / 2;
		pts[2].x = cx + s; pts[2].y = cy - s / 2;
		break;
	case 2:
		pts[0].x = cx - s; pts[0].y = cy;
		pts[1].x = cx + s / 2; pts[1].y = cy - s;
		pts[2].x = cx + s / 2; pts[2].y = cy + s;
		break;
	default:
		pts[0].x = cx + s; pts[0].y = cy;
		pts[1].x = cx - s / 2; pts[1].y = cy - s;
		pts[2].x = cx - s / 2; pts[2].y = cy + s;
		break;
	}
	CBrush br( CHostThemeHelper::SoftGlyphColor() );
	CBrush* pOldBrush = pDC->SelectObject( &br );
	CPen* pOldPen = (CPen*)pDC->SelectStockObject( NULL_PEN );
	pDC->Polygon( pts, 3 );
	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldBrush );
}

void PaintNcScrollBar( HDC hdc, HWND hwnd, RECT rc, bool bVert )
{
	const int nW = rc.right - rc.left;
	const int nH = rc.bottom - rc.top;
	if( nW < 8 || nH < 8 )
		return;
	const int nArrow = bVert ? nW : nH;
	RECT rcA1 = rc;
	RECT rcA2 = rc;
	RECT rcTrack = rc;
	if( bVert )
	{
		rcA1.bottom = rc.top + nArrow;
		rcA2.top = rc.bottom - nArrow;
		rcTrack.top = rcA1.bottom;
		rcTrack.bottom = rcA2.top;
	}
	else
	{
		rcA1.right = rc.left + nArrow;
		rcA2.left = rc.right - nArrow;
		rcTrack.left = rcA1.right;
		rcTrack.right = rcA2.left;
	}
	FillEdgeBand( hdc, rcTrack.left, rcTrack.top,
		rcTrack.right - rcTrack.left, rcTrack.bottom - rcTrack.top, OdclSysColor( COLOR_3DSHADOW ) );
	DrawNcScrollArrow( hdc, rcA1, bVert ? 0 : 2 );
	DrawNcScrollArrow( hdc, rcA2, bVert ? 1 : 3 );

	SCROLLINFO si = {};
	si.cbSize = sizeof( si );
	si.fMask = SIF_ALL;
	if( !::GetScrollInfo( hwnd, bVert ? SB_VERT : SB_HORZ, &si ) )
		return;
	const int nRange = si.nMax - si.nMin + 1;
	const int nPage = (int)max( (UINT)1, si.nPage );
	const int nTrack = bVert ? (rcTrack.bottom - rcTrack.top) : (rcTrack.right - rcTrack.left);
	if( nRange <= nPage || nTrack <= 0 )
		return;
	int nThumb = max( nArrow, MulDiv( nPage, nTrack, nRange ) );
	if( nThumb > nTrack )
		nThumb = nTrack;
	const int nTravel = nTrack - nThumb;
	const int nMaxPos = max( 1, nRange - nPage );
	const int nOffset = MulDiv( si.nPos - si.nMin, nTravel, nMaxPos );
	RECT rcThumb = rcTrack;
	if( bVert )
	{
		rcThumb.top = rcTrack.top + nOffset;
		rcThumb.bottom = rcThumb.top + nThumb;
	}
	else
	{
		rcThumb.left = rcTrack.left + nOffset;
		rcThumb.right = rcThumb.left + nThumb;
	}
	FillEdgeBand( hdc, rcThumb.left, rcThumb.top,
		rcThumb.right - rcThumb.left, rcThumb.bottom - rcThumb.top, OdclSysColor( COLOR_BTNFACE ) );
	FrameRectColor( hdc, rcThumb, OdclSysColor( COLOR_3DSHADOW ) );
}

} // namespace

void CHostThemeHelper::PaintEtchedRect( HDC hdc, const RECT& rc )
{
	if( !hdc )
		return;
	const int w = rc.right - rc.left;
	const int h = rc.bottom - rc.top;
	if( w <= 0 || h <= 0 )
		return;
	const COLORREF crSh = OdclSysColor( COLOR_3DSHADOW );
	const COLORREF crDk = OdclSysColor( COLOR_3DDKSHADOW );
	FillEdgeBand( hdc, rc.left, rc.top, w - 1, 1, crSh );
	FillEdgeBand( hdc, rc.left, rc.top, 1, h - 1, crSh );
	FillEdgeBand( hdc, rc.left, rc.bottom - 1, w, 1, crDk );
	FillEdgeBand( hdc, rc.right - 1, rc.top, 1, h, crDk );
}

void CHostThemeHelper::PaintRaisedInner( HDC hdc, const RECT& rc )
{
	if( !hdc )
		return;
	const int w = rc.right - rc.left;
	const int h = rc.bottom - rc.top;
	if( w <= 0 || h <= 0 )
		return;
	const COLORREF crSh = OdclSysColor( COLOR_3DSHADOW );
	FillEdgeBand( hdc, rc.left, rc.bottom - 1, w, 1, crSh );
	FillEdgeBand( hdc, rc.right - 1, rc.top, 1, h, crSh );
}

namespace {

LRESULT CALLBACK HostNcBorderWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	WNDPROC pfnOld = (WNDPROC)::GetProp( hWnd, kNcBorderOldProcProp );
	if( !pfnOld )
		return ::DefWindowProc( hWnd, uMsg, wParam, lParam );

	if( uMsg == WM_NCDESTROY )
	{
		::SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR)pfnOld );
		::RemoveProp( hWnd, kNcBorderOldProcProp );
		return ::CallWindowProc( pfnOld, hWnd, uMsg, wParam, lParam );
	}
	if( uMsg == WM_NCPAINT || uMsg == WM_HSCROLL || uMsg == WM_VSCROLL || uMsg == WM_MOUSEWHEEL )
	{
		const LRESULT lr = ::CallWindowProc( pfnOld, hWnd, uMsg, wParam, lParam );
		CHostThemeHelper::PaintNcBorder( hWnd );
		return lr;
	}
	if( uMsg == WM_PAINT )
	{
		const LRESULT lr = ::CallWindowProc( pfnOld, hWnd, uMsg, wParam, lParam );
		TCHAR szCls[32] = {};
		::GetClassName( hWnd, szCls, 32 );
		if( !lstrcmpi( szCls, _T("ComboBox") ) || !lstrcmpi( szCls, _T("ComboBoxEx32") ) )
			CHostThemeHelper::PaintComboChrome( hWnd );
		return lr;
	}
	return ::CallWindowProc( pfnOld, hWnd, uMsg, wParam, lParam );
}

} // namespace

void CHostThemeHelper::InstallNcBorder( HWND hwnd )
{
	if( !hwnd || !::IsWindow( hwnd ) )
		return;
	if( ::GetProp( hwnd, kNcBorderOldProcProp ) )
		return;
	WNDPROC pfnOld = (WNDPROC)::SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)HostNcBorderWndProc );
	if( pfnOld )
		::SetProp( hwnd, kNcBorderOldProcProp, (HANDLE)pfnOld );
}

void CHostThemeHelper::InstallNcBorderTree( HWND hwndRoot )
{
	if( !hwndRoot )
		return;
	InstallNcBorder( hwndRoot );
	for( HWND hwndChild = ::FindWindowEx( hwndRoot, NULL, NULL, NULL );
			 hwndChild;
			 hwndChild = ::FindWindowEx( hwndRoot, hwndChild, NULL, NULL ) )
		InstallNcBorderTree( hwndChild );
}

bool CHostThemeHelper::PaintNcBorder( HWND hwnd )
{
	if( !hwnd || !HostMaps() )
		return false;

	const DWORD dwEx = (DWORD)::GetWindowLong( hwnd, GWL_EXSTYLE );
	const DWORD dwStyle = (DWORD)::GetWindowLong( hwnd, GWL_STYLE );
	const bool bClient = (dwEx & WS_EX_CLIENTEDGE) != 0;
	const bool bStatic = (dwEx & WS_EX_STATICEDGE) != 0;
	const bool bBorder = (dwStyle & WS_BORDER) != 0;
	const bool bHScroll = (dwStyle & WS_HSCROLL) != 0;
	const bool bVScroll = (dwStyle & WS_VSCROLL) != 0;
	if( !bClient && !bStatic && !bBorder && !bHScroll && !bVScroll )
		return false;

	RECT rcWnd = {};
	::GetWindowRect( hwnd, &rcWnd );
	const int cx = rcWnd.right - rcWnd.left;
	const int cy = rcWnd.bottom - rcWnd.top;
	if( cx <= 0 || cy <= 0 )
		return false;

	HDC hdc = ::GetWindowDC( hwnd );
	if( !hdc )
		return false;

	if( bHScroll || bVScroll )
	{
		RECT rcC = {};
		::GetClientRect( hwnd, &rcC );
		POINT pt = { rcC.left, rcC.top };
		::ClientToScreen( hwnd, &pt );
		const int x = pt.x - rcWnd.left;
		const int y = pt.y - rcWnd.top;
		const RECT rcClientWnd = { x, y, x + (rcC.right - rcC.left), y + (rcC.bottom - rcC.top) };
		int nBtm = 0;
		int nRgt = 0;
		if( bClient )
			nBtm = nRgt = 2;
		else if( bStatic || bBorder )
			nBtm = nRgt = 1;
		if( bHScroll )
		{
			RECT rcH = { rcClientWnd.left, rcClientWnd.bottom, rcClientWnd.right, cy - nBtm };
			PaintNcScrollBar( hdc, hwnd, rcH, false );
		}
		if( bVScroll )
		{
			RECT rcV = { rcClientWnd.right, rcClientWnd.top, cx - nRgt, rcClientWnd.bottom };
			PaintNcScrollBar( hdc, hwnd, rcV, true );
		}
		if( rcClientWnd.right < cx && rcClientWnd.bottom < cy )
			FillEdgeBand( hdc, rcClientWnd.right, rcClientWnd.bottom,
				cx - rcClientWnd.right, cy - rcClientWnd.bottom, OdclSysColor( COLOR_BTNFACE ) );
	}

	RECT rc = { 0, 0, cx, cy };
	if( bClient )
	{
		// Sunken well, no light bevel (system BTNHIGHLIGHT is too bright on dark CAD).
		FrameRectColor( hdc, rc, OdclSysColor( COLOR_3DSHADOW ) );
		::InflateRect( &rc, -1, -1 );
		FrameRectColor( hdc, rc, OdclSysColor( COLOR_3DDKSHADOW ) );
	}
	else if( bStatic )
		FrameRectColor( hdc, rc, OdclSysColor( COLOR_3DSHADOW ) );
	else
		FrameRectColor( hdc, rc, OdclSysColor( COLOR_WINDOWFRAME ) );

	::ReleaseDC( hwnd, hdc );
	return true;
}

#endif // ODCL_HOST_COLORTHEME
