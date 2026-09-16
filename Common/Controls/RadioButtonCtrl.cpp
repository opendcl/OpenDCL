// RadioButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "RadioButtonCtrl.h"
#include "DclControlTemplate.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "PropertyIds.h"
#include "HostThemeHelper.h"
#include "ColorService.h"
#include "MemDC.h"


//from CommCtrl.h
#define BCN_FIRST               (0U-1250U)
#define BCN_HOTITEMCHANGE       (BCN_FIRST + 0x0001)
#ifndef HICF_ENTERING
#define HICF_ENTERING           0x0010
#endif
#ifndef HICF_LEAVING
#define HICF_LEAVING            0x0020
#endif


/////////////////////////////////////////////////////////////////////////////
// CRadioButtonCtrl

CRadioButtonCtrl::CRadioButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mbTrackingMouse( false )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CRadioButtonCtrl::~CRadioButtonCtrl()
{
}

bool CRadioButtonCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess )
		SyncHostOptionTheme();

	return bSuccess;
}

DWORD CRadioButtonCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (/*WS_CLIPSIBLINGS | */BS_AUTORADIOBUTTON | BS_NOTIFY);
	return dwStyle;
}

bool CRadioButtonCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Caption:
		SetWindowText( pProp->GetStringValue() );
		break;
	case Prop::Value:
		SetCheck( pProp->GetLongValue() );
		break;
	}
	return !bFailed;
}

bool CRadioButtonCtrl::UseHostOwnerDraw() const
{
	return CHostThemeHelper::HostMaps() || !mpTemplate->GetBooleanProperty( Prop::UseVisualStyle );
}

void CRadioButtonCtrl::HandleHostThemeChanged()
{
	SyncHostOptionTheme();
}

bool CRadioButtonCtrl::OnApplyUseVisualStyle( TPropertyPtr pProp )
{
	if( !__super::OnApplyUseVisualStyle( pProp ) )
		return false;
	SyncHostOptionTheme();
	return true;
}

void CRadioButtonCtrl::SyncHostOptionTheme()
{
	if( !m_hWnd )
		return;
	LPCWSTR pszTheme = CHostThemeHelper::ThemeClass( mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) );
	GetTheme().SetWindowTheme( pszTheme, pszTheme );
	CHostThemeHelper::Apply( m_hWnd, pszTheme );
	OnNeedRepaint( true );
}

void CRadioButtonCtrl::PaintHostOption( CDC* pDC )
{
	if( !pDC )
		return;

	CRect rcClient;
	GetClientRect( &rcClient );

	COLORREF crBk = OdclSysColor( COLOR_BTNFACE );
	CAcadColorService* pColorService = GetColorService();
	if( pColorService && !pColorService->IsBackgroundNotSet() && !pColorService->IsBackgroundTransparent() )
		crBk = pColorService->GetBackgroundColor();
	pDC->FillSolidRect( &rcClient, crBk );

	const bool bEnabled = (IsWindowEnabled() != FALSE);
	const int nCheck = GetCheck();
	const int nBox = (int)FromDIP( 13 );
	const int nGap = (int)FromDIP( 4 );
	const int nPad = (int)FromDIP( 1 );
	int yBox = rcClient.top + (rcClient.Height() - nBox) / 2;
	if( yBox < rcClient.top )
		yBox = rcClient.top;
	CRect rcBox( rcClient.left + nPad, yBox, rcClient.left + nPad + nBox, yBox + nBox );

	const COLORREF crRing = OdclSysColor( bEnabled ? COLOR_BTNTEXT : COLOR_GRAYTEXT );
	CPen penRing( PS_SOLID, 1, crRing );
	CBrush brFill( OdclSysColor( COLOR_WINDOW ) );
	CPen* pOldPen = pDC->SelectObject( &penRing );
	CBrush* pOldBrush = pDC->SelectObject( &brFill );
	pDC->Ellipse( &rcBox );
	if( nCheck == BST_CHECKED )
	{
		CRect rcDot = rcBox;
		rcDot.DeflateRect( nBox / 4, nBox / 4 );
		CBrush brDot( crRing );
		pDC->SelectObject( &brDot );
		pDC->Ellipse( &rcDot );
		pDC->SelectObject( pOldBrush );
	}
	pDC->SelectObject( pOldBrush );
	pDC->SelectObject( pOldPen );

	CString sCaption;
	GetWindowText( sCaption );
	CRect rcText = rcClient;
	rcText.left = rcBox.right + nGap;
	COLORREF crText = crRing;
	if( bEnabled && pColorService )
		crText = pColorService->GetForegroundColor();
	const COLORREF crOld = pDC->SetTextColor( crText );
	const int nOldBk = pDC->SetBkMode( TRANSPARENT );
	CFont* pFont = GetFont();
	CFont* pOldFont = pFont ? pDC->SelectObject( pFont ) : NULL;
	pDC->DrawText( sCaption, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS );
	if( GetFocus() == this )
	{
		CRect rcFocus = rcText;
		pDC->DrawText( sCaption, &rcFocus, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_CALCRECT );
		rcFocus.InflateRect( (int)FromDIP( 1 ), (int)FromDIP( 1 ) );
		pDC->DrawFocusRect( &rcFocus );
	}
	if( pOldFont )
		pDC->SelectObject( pOldFont );
	pDC->SetBkMode( nOldBk );
	pDC->SetTextColor( crOld );
}

BEGIN_MESSAGE_MAP(CRadioButtonCtrl, CButton)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(BCN_HOTITEMCHANGE, &CRadioButtonCtrl::OnBnHotItemChange)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, &CRadioButtonCtrl::OnMouseLeave)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CRadioButtonCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRadioButtonCtrl message handlers

LRESULT CRadioButtonCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

LRESULT CRadioButtonCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = __super::WindowProc(message, wParam, lParam);
	if( message == BM_SETCHECK )
		mpTemplate->SetLongProperty( Prop::Value, wParam );
	return lResult;
}

BOOL CRadioButtonCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
		pMsg->wParam = VK_TAB;		
	return __super::PreTranslateMessage(pMsg);
}

void CRadioButtonCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus(pOldWnd);
	OnNeedRepaint();
}

void CRadioButtonCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	EndHoverTracking();
	__super::OnKillFocus(pNewWnd);
}

HBRUSH CRadioButtonCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbrBackground = HandleCtlColor( pDC, nCtlColor );
	if( hbrBackground )
		return hbrBackground;
	if( GetTheme().IsThemeActive() )
		return NULL; //when using visual style, transparent brush causes class background to be used
	return CAcadColorService::GetTransparentBrush();
}

BOOL CRadioButtonCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( UseHostOwnerDraw() && pDC )
	{
		CRect rc;
		GetClientRect( &rc );
		pDC->FillSolidRect( &rc, OdclSysColor( COLOR_BTNFACE ) );
		return TRUE;
	}
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CRadioButtonCtrl::OnPaint()
{
	if( !UseHostOwnerDraw() )
	{
		Default();
		return;
	}
	CPaintDC dcPaint( this );
	CRect rcClient;
	GetClientRect( &rcClient );
	CMemDCx dc( &dcPaint, rcClient );
	PaintHostOption( &dc );
}

void CRadioButtonCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CRadioButtonCtrl::StartHoverTracking()
{
	if( mbTrackingMouse )
		return;
	TRACKMOUSEEVENT tm = { sizeof( TRACKMOUSEEVENT ), TME_LEAVE, m_hWnd, 0 };
	if( _TrackMouseEvent( &tm ) )
		mbTrackingMouse = true;
	SetTimer( kMouseLeaveTimer, 100, NULL );
}

void CRadioButtonCtrl::EraseHotFill()
{
	CWnd* pParent = GetParent();
	if( pParent && pParent->m_hWnd )
	{
		CRect rc;
		GetWindowRect( &rc );
		pParent->ScreenToClient( &rc );
		pParent->RedrawWindow( &rc, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW | RDW_NOCHILDREN );
	}
	Invalidate();
	UpdateWindow();
}

void CRadioButtonCtrl::EndHoverTracking()
{
	if( mbTrackingMouse )
	{
		TRACKMOUSEEVENT tm = { sizeof( TRACKMOUSEEVENT ), TME_LEAVE | TME_CANCEL, m_hWnd, 0 };
		_TrackMouseEvent( &tm );
	}
	mbTrackingMouse = false;
	KillTimer( kMouseLeaveTimer );
	EraseHotFill();
}

void CRadioButtonCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove( nFlags, point );
	StartHoverTracking();
}

LRESULT CRadioButtonCtrl::OnMouseLeave(WPARAM, LPARAM)
{
	EndHoverTracking();
	return 0;
}

#if (_MFC_VER < 0x0800)
void CRadioButtonCtrl::OnTimer(UINT nIDEvent)
#else
void CRadioButtonCtrl::OnTimer(UINT_PTR nIDEvent)
#endif
{
	if( nIDEvent != kMouseLeaveTimer )
	{
		__super::OnTimer( nIDEvent );
		return;
	}
	CPoint ptCursor;
	if( !GetCursorPos( &ptCursor ) )
		return;
	CRect rcWnd;
	GetWindowRect( &rcWnd );
	if( !rcWnd.PtInRect( ptCursor ) )
		EndHoverTracking();
}

void CRadioButtonCtrl::OnBnHotItemChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	struct HotItem { NMHDR hdr; DWORD dwFlags; };
	HotItem* pHot = reinterpret_cast<HotItem*>( pNMHDR );
	if( !pHot )
		return;
	if( pHot->dwFlags & HICF_ENTERING )
	{
		StartHoverTracking();
		return;
	}
	if( !( pHot->dwFlags & HICF_LEAVING ) )
		return;
	CPoint ptCursor;
	if( GetCursorPos( &ptCursor ) )
	{
		CRect rcWnd;
		GetWindowRect( &rcWnd );
		if( rcWnd.PtInRect( ptCursor ) )
			return;
	}
	EndHoverTracking();
}
