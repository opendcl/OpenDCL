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
	CPen pen( PS_SOLID, 1, OdclSysColor( bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT ) );
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
	CBrush br( OdclSysColor( bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT ) );
	pOldBrush = pDC->SelectObject( &br );
	pOldPen = (CPen*)pDC->SelectStockObject( NULL_PEN );
	pDC->Polygon( pts, 3 );
	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldBrush );
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
	if( uMsg == WM_NCPAINT )
	{
		const LRESULT lr = ::CallWindowProc( pfnOld, hWnd, uMsg, wParam, lParam );
		CHostThemeHelper::PaintNcBorder( hWnd );
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
	if( !bClient && !bStatic && !bBorder )
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
