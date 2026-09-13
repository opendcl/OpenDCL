// FrameCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FrameCtrl.h"
#include "ControlPane.h"
#include "ColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CFrameCtrl

CFrameCtrl::CFrameCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mColorService()
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CFrameCtrl::~CFrameCtrl()
{
}

bool CFrameCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess )
		ModifyStyleEx( 0, WS_EX_TRANSPARENT );

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CFrameCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (WS_CLIPSIBLINGS | BS_GROUPBOX);
	return dwStyle;
}


BEGIN_MESSAGE_MAP(CFrameCtrl, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_WINDOWPOSCHANGING()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CFrameCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFrameCtrl message handlers

LRESULT CFrameCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

BOOL CFrameCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CFrameCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbrBackground = HandleCtlColor( pDC, nCtlColor );
	if( hbrBackground )
		return hbrBackground;
	const COLORREF crFace = OdclSysColor( COLOR_BTNFACE );
	pDC->SetTextColor( mColorService.GetForegroundColor() );
	pDC->SetBkColor( crFace );
	pDC->SetBkMode( OPAQUE );
	static CBrush brFace;
	static COLORREF crCached = (COLORREF)-1;
	if( crCached != crFace || !(HBRUSH)brFace )
	{
		brFace.DeleteObject();
		brFace.CreateSolidBrush( crFace );
		crCached = crFace;
	}
	return brFace;
}

BOOL CFrameCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return TRUE;
}

void CFrameCtrl::OnPaint()
{
	CPaintDC dc( this );
	CRect rc;
	GetClientRect( &rc );
	CString sCaption;
	GetWindowText( sCaption );
	CFont* pOldFont = dc.SelectObject( GetFont() );
	const COLORREF crFace = OdclSysColor( COLOR_BTNFACE );
	const COLORREF crText = IsWindowEnabled() ? mColorService.GetForegroundColor() : OdclSysColor( COLOR_GRAYTEXT );
	const DWORD dwStyle = GetStyle();
	UINT dt = DT_SINGLELINE | DT_VCENTER;
	if( dwStyle & BS_RIGHT )
		dt |= DT_RIGHT;
	else if( dwStyle & BS_CENTER )
		dt |= DT_CENTER;
	else
		dt |= DT_LEFT;
	if( GetExStyle() & WS_EX_RTLREADING )
		dt |= DT_RTLREADING;
	if( SendMessage( WM_QUERYUISTATE ) & UISF_HIDEACCEL )
		dt |= DT_HIDEPREFIX;
	CRect rcText( 0, 0, 0, 0 );
	TEXTMETRIC tm = { 0 };
	dc.GetTextMetrics( &tm );
	if( !sCaption.IsEmpty() )
		dc.DrawText( sCaption, &rcText, dt | DT_CALCRECT );
	const int nCy = max( rcText.Height(), tm.tmHeight );
	CRect rcBox( rc );
	rcBox.top += nCy / 2;
	dc.DrawEdge( &rcBox, EDGE_ETCHED, BF_RECT );
	if( !sCaption.IsEmpty() )
	{
		const int nPad = FromDIP( 8 );
		const int nGap = FromDIP( 2 );
		const int nCx = rcText.Width();
		rcText.top = 0;
		rcText.bottom = nCy;
		if( dt & DT_CENTER )
		{
			rcText.left = ( rc.Width() - nCx ) / 2;
			rcText.right = rcText.left + nCx;
		}
		else if( dt & DT_RIGHT )
		{
			rcText.right = rc.right - nPad;
			rcText.left = rcText.right - nCx;
		}
		else
		{
			rcText.left = nPad;
			rcText.right = nPad + nCx;
		}
		CRect rcMask( rcText );
		rcMask.InflateRect( nGap, FromDIP( 1 ) );
		dc.FillSolidRect( &rcMask, crFace );
		dc.SetBkColor( crFace );
		dc.SetTextColor( crText );
		dc.SetBkMode( OPAQUE );
		dc.DrawText( sCaption, &rcText, dt );
	}
	dc.SelectObject( pOldFont );
}

void CFrameCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CFrameCtrl::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//lpwndpos->flags |= SWP_NOZORDER;
	__super::OnWindowPosChanging(lpwndpos);
}
