// TextButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TextButtonCtrl.h"
#include "DclControlTemplate.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "PropertyIds.h"
#include "ColorService.h"
#include <math.h>


/////////////////////////////////////////////////////////////////////////////
// CTextButtonCtrl

CTextButtonCtrl::CTextButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, m_bIsDefault( FALSE )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CTextButtonCtrl::~CTextButtonCtrl()
{
}

bool CTextButtonCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CTextButtonCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (BS_OWNERDRAW | BS_MULTILINE | BS_NOTIFY);
	return dwStyle;
}

bool CTextButtonCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Caption:
		{
			SetWindowText( pProp->GetStringValue() );
		}
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CTextButtonCtrl, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_GETDLGCODE()
	ON_MESSAGE(BM_SETSTYLE, &CTextButtonCtrl::OnSetStyle)
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CTextButtonCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextButtonCtrl message handlers

LRESULT CTextButtonCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

BOOL CTextButtonCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CTextButtonCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CTextButtonCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CTextButtonCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CTextButtonCtrl::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus(pOldWnd);
	Invalidate();
}

void CTextButtonCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	Invalidate();
}

UINT CTextButtonCtrl::OnGetDlgCode()
{
	UINT nCode = CButton::OnGetDlgCode();
	nCode |= ( m_bIsDefault ? DLGC_DEFPUSHBUTTON : DLGC_UNDEFPUSHBUTTON );
	return nCode;
}

LRESULT CTextButtonCtrl::OnSetStyle(WPARAM wParam, LPARAM lParam)
{
	const UINT nNewType = (UINT)( wParam & BS_TYPEMASK );
	if( nNewType == BS_DEFPUSHBUTTON )
		m_bIsDefault = TRUE;
	else if( nNewType == BS_PUSHBUTTON )
		m_bIsDefault = FALSE;
	return DefWindowProc( BM_SETSTYLE, ( wParam & ~BS_TYPEMASK ) | BS_OWNERDRAW, lParam );
}

static COLORREF MixRgb( COLORREF a, COLORREF b, int nNum, int nDen )
{
	return RGB(
		( GetRValue( a ) * ( nDen - nNum ) + GetRValue( b ) * nNum ) / nDen,
		( GetGValue( a ) * ( nDen - nNum ) + GetGValue( b ) * nNum ) / nDen,
		( GetBValue( a ) * ( nDen - nNum ) + GetBValue( b ) * nNum ) / nDen );
}

static float Cover( float sd )
{
	const float a = 0.5f - sd;
	if( a <= 0.0f )
		return 0.0f;
	if( a >= 1.0f )
		return 1.0f;
	return a;
}

static float SdRoundRect( float x, float y, float w, float h, float r )
{
	const float px = fabsf( x - w * 0.5f );
	const float py = fabsf( y - h * 0.5f );
	float qx = px - ( w * 0.5f - r );
	float qy = py - ( h * 0.5f - r );
	const float inside = ( qx > qy ? qx : qy );
	if( qx < 0.0f )
		qx = 0.0f;
	if( qy < 0.0f )
		qy = 0.0f;
	const float outside = sqrtf( qx * qx + qy * qy );
	return ( inside < 0.0f ? inside : 0.0f ) + outside - r;
}

static COLORREF BlendRgb( COLORREF a, COLORREF b, float t )
{
	const int n = (int)( t * 256.0f + 0.5f );
	return RGB(
		( GetRValue( a ) * ( 256 - n ) + GetRValue( b ) * n ) >> 8,
		( GetGValue( a ) * ( 256 - n ) + GetGValue( b ) * n ) >> 8,
		( GetBValue( a ) * ( 256 - n ) + GetBValue( b ) * n ) >> 8 );
}

static void DrawRoundRectAA( CDC* pDC, const CRect& rc, int nR, COLORREF crFill, COLORREF crBorder, COLORREF crBk )
{
	const int cx = rc.Width();
	const int cy = rc.Height();
	if( cx <= 0 || cy <= 0 )
		return;
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bmi.bmiHeader.biWidth = cx;
	bmi.bmiHeader.biHeight = -cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	void* pBits = NULL;
	HBITMAP hbm = CreateDIBSection( pDC->GetSafeHdc(), &bmi, DIB_RGB_COLORS, &pBits, NULL, 0 );
	if( !hbm || !pBits )
		return;
	const float w = (float)cx;
	const float h = (float)cy;
	float r = (float)nR;
	if( r > w * 0.5f )
		r = w * 0.5f;
	if( r > h * 0.5f )
		r = h * 0.5f;
	DWORD* pPix = (DWORD*)pBits;
	for( int y = 0; y < cy; ++y )
	{
		for( int x = 0; x < cx; ++x )
		{
			const float sd = SdRoundRect( x + 0.5f, y + 0.5f, w, h, r );
			const float aOuter = Cover( sd );
			const float aInner = Cover( sd + 1.0f );
			COLORREF cr = crBk;
			if( aOuter > 0.0f )
				cr = BlendRgb( crBk, crBorder, aOuter );
			if( aInner > 0.0f )
				cr = BlendRgb( cr, crFill, aInner / ( aOuter > 0.0f ? aOuter : 1.0f ) );
			pPix[y * cx + x] = 0xFF000000u | ( GetBValue( cr ) ) | ( GetGValue( cr ) << 8 ) | ( GetRValue( cr ) << 16 );
		}
	}
	HDC hdcMem = CreateCompatibleDC( pDC->GetSafeHdc() );
	HGDIOBJ hOld = SelectObject( hdcMem, hbm );
	BitBlt( pDC->GetSafeHdc(), rc.left, rc.top, cx, cy, hdcMem, 0, 0, SRCCOPY );
	SelectObject( hdcMem, hOld );
	DeleteDC( hdcMem );
	DeleteObject( hbm );
}

void CTextButtonCtrl::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle( lpDIS->hDC );
	CRect rc( lpDIS->rcItem );
	const bool bPressed = ( lpDIS->itemState & ODS_SELECTED ) != 0;
	const bool bFocus = ( lpDIS->itemState & ODS_FOCUS ) != 0;
	const bool bDisabled = ( lpDIS->itemState & ODS_DISABLED ) != 0;

	COLORREF crBase = mColorService.GetBackgroundColor();
	if( mColorService.IsBackgroundNotSet() || mColorService.IsBackgroundTransparent() )
		crBase = OdclSysColor( COLOR_BTNFACE );
	COLORREF crFace = MixRgb( crBase, OdclSysColor( COLOR_BTNHIGHLIGHT ), 2, 3 );
	COLORREF crBorder = OdclSysColor( COLOR_3DDKSHADOW );
	if( bPressed )
	{
		crFace = MixRgb( crBase, OdclSysColor( COLOR_BTNSHADOW ), 1, 3 );
		crBorder = MixRgb( crBorder, RGB( 0, 0, 0 ), 1, 3 );
	}
	const COLORREF crText = bDisabled ? OdclSysColor( COLOR_GRAYTEXT ) : mColorService.GetForegroundColor();

	const COLORREF crBk = OdclSysColor( COLOR_BTNFACE );
	int nR = FromDIP( 4 );
	DrawRoundRectAA( pDC, rc, nR, crFace, crBorder, crBk );

	CString sCaption;
	GetWindowText( sCaption );
	CRect rcText( rc );
	rcText.DeflateRect( FromDIP( 4 ), FromDIP( 2 ) );

	CFont* pOldFont = pDC->SelectObject( GetFont() );
	UINT dt = DT_WORDBREAK | DT_CENTER;
	if( GetExStyle() & WS_EX_RTLREADING )
		dt |= DT_RTLREADING;
	if( SendMessage( WM_QUERYUISTATE ) & UISF_HIDEACCEL )
		dt |= DT_HIDEPREFIX;

	CRect rcCalc( rcText );
	pDC->DrawText( sCaption, &rcCalc, dt | DT_CALCRECT );
	rcText.top += ( rcText.Height() - rcCalc.Height() ) / 2;
	rcText.bottom = rcText.top + rcCalc.Height();

	pDC->SetBkMode( TRANSPARENT );
	pDC->SetTextColor( crText );
	pDC->DrawText( sCaption, &rcText, dt );
	pDC->SelectObject( pOldFont );
}
