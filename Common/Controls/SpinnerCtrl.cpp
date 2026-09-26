// SpinnerCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SpinnerCtrl.h"
#include "ControlPane.h"
#include "HostThemeHelper.h"
#include "ColorService.h"
#include "MemDC.h"


/////////////////////////////////////////////////////////////////////////////
// CSpinnerCtrl

CSpinnerCtrl::CSpinnerCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CSpinnerCtrl::~CSpinnerCtrl()
{
}

bool CSpinnerCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);
	if( bSuccess )
	{
	#ifdef _UNICODE
		BOOL bUnicode = TRUE;
	#else
		BOOL bUnicode = FALSE;
	#endif
		SendMessage( CCM_SETUNICODEFORMAT, (WPARAM)bUnicode, 0 );
	}

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess )
		SyncHostSpinTheme();

	return bSuccess;
}

DWORD CSpinnerCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (UDS_ARROWKEYS);

	if( mpTemplate->GetLongProperty( Prop::Orientation ) == 0 )
		dwStyle |= UDS_HORZ;

	if( mpTemplate->GetBooleanProperty( Prop::AutoWrap ) )
		dwStyle |= UDS_WRAP;

	return dwStyle;
}

bool CSpinnerCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Value:
		SetPos( pProp->GetLongValue() );
		break;
	case Prop::MinValue:
		SetRange( (short)pProp->GetLongValue(), (short)mpTemplate->GetLongProperty( Prop::MaxValue ) );
		break;
	case Prop::MaxValue:
		if( !IsEnumeratingProperties() )
			SetRange( (short)mpTemplate->GetLongProperty( Prop::MinValue ), (short)pProp->GetLongValue() );
		break;
	case Prop::Orientation:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( 0, UDS_HORZ, SWP_FRAMECHANGED );
		else
			ModifyStyle( UDS_HORZ, 0, SWP_FRAMECHANGED );
		break;
	case Prop::AutoWrap:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, UDS_WRAP );
		else
			ModifyStyle( UDS_WRAP, 0 );
		break;
	}
	return !bFailed;
}

bool CSpinnerCtrl::UseHostOwnerDraw() const
{
	return CHostThemeHelper::HostMaps() || !mpTemplate->GetBooleanProperty( Prop::UseVisualStyle );
}

void CSpinnerCtrl::HandleHostThemeChanged()
{
	SyncHostSpinTheme();
}

bool CSpinnerCtrl::OnApplyUseVisualStyle( TPropertyPtr pProp )
{
	if( !__super::OnApplyUseVisualStyle( pProp ) )
		return false;
	SyncHostSpinTheme();
	return true;
}

void CSpinnerCtrl::SyncHostSpinTheme()
{
	if( !m_hWnd )
		return;
	LPCWSTR pszTheme = CHostThemeHelper::ThemeClass( mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) );
	GetTheme().SetWindowTheme( pszTheme, pszTheme );
	CHostThemeHelper::Apply( m_hWnd, pszTheme );
	OnNeedRepaint( true );
}

static void DrawSpinArrow( CDC* pDC, const CRect& rc, int nDir )
{
	pDC->FillSolidRect( &rc, OdclSysColor( COLOR_BTNFACE ) );
	CPen pen( PS_SOLID, 1, OdclSysColor( COLOR_3DSHADOW ) );
	CPen* pOldPen = pDC->SelectObject( &pen );
	CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( NULL_BRUSH );
	pDC->Rectangle( rc.left, rc.top, rc.right, rc.bottom );
	pDC->SelectObject( pOldBrush );
	pDC->SelectObject( pOldPen );

	const int cx = (rc.left + rc.right) / 2;
	const int cy = (rc.top + rc.bottom) / 2;
	const int s = max( 2, min( rc.Width(), rc.Height() ) / 4 );
	POINT pts[3] = {0};
	switch( nDir )
	{
	case 0: // up
		pts[0].x = cx; pts[0].y = cy - s;
		pts[1].x = cx - s; pts[1].y = cy + s / 2;
		pts[2].x = cx + s; pts[2].y = cy + s / 2;
		break;
	case 1: // down
		pts[0].x = cx; pts[0].y = cy + s;
		pts[1].x = cx - s; pts[1].y = cy - s / 2;
		pts[2].x = cx + s; pts[2].y = cy - s / 2;
		break;
	case 2: // left
		pts[0].x = cx - s; pts[0].y = cy;
		pts[1].x = cx + s / 2; pts[1].y = cy - s;
		pts[2].x = cx + s / 2; pts[2].y = cy + s;
		break;
	default: // right
		pts[0].x = cx + s; pts[0].y = cy;
		pts[1].x = cx - s / 2; pts[1].y = cy - s;
		pts[2].x = cx - s / 2; pts[2].y = cy + s;
		break;
	}
	CBrush br( CHostThemeHelper::SoftGlyphColor() );
	pOldBrush = pDC->SelectObject( &br );
	pOldPen = (CPen*)pDC->SelectStockObject( NULL_PEN );
	pDC->Polygon( pts, 3 );
	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldBrush );
}

void CSpinnerCtrl::PaintHostSpin( CDC* pDC )
{
	if( !pDC )
		return;
	CRect rc;
	GetClientRect( &rc );
	const bool bHorz = (GetStyle() & UDS_HORZ) != 0;
	CRect rc1 = rc;
	CRect rc2 = rc;
	if( bHorz )
	{
		rc1.right = rc.left + rc.Width() / 2;
		rc2.left = rc1.right;
	}
	else
	{
		rc1.bottom = rc.top + rc.Height() / 2;
		rc2.top = rc1.bottom;
	}
	DrawSpinArrow( pDC, rc1, bHorz ? 2 : 0 );
	DrawSpinArrow( pDC, rc2, bHorz ? 3 : 1 );
}

BEGIN_MESSAGE_MAP(CSpinnerCtrl, CSpinButtonCtrl)
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltapos)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CSpinnerCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSpinnerCtrl message handlers

LRESULT CSpinnerCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

void CSpinnerCtrl::OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nMinPos = mpTemplate->GetLongProperty( Prop::MinValue );
	int nMaxPos = mpTemplate->GetLongProperty( Prop::MaxValue );
	int nNewPos = mpTemplate->GetLongProperty( Prop::Value );

	nNewPos += pNMUpDown->iDelta;
	if( mpTemplate->GetBooleanProperty( Prop::AutoWrap ) )
	{	
		if( nNewPos < nMinPos )
			nNewPos = nMaxPos;
		else if( nNewPos > nMaxPos )
			nNewPos = nMinPos;				
	}
	else
	{	
		if( nNewPos < nMinPos )
			nNewPos = nMinPos;
		else if( nNewPos > nMaxPos )
			nNewPos = nMaxPos;				
	}

	TPropertyPtr pValueProp = mpTemplate->GetPropertyObject( Prop::Value );
	pValueProp->SetLongValue( nNewPos );
	ApplyProperty( pValueProp );

	*pResult = 0;
}

void CSpinnerCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CSpinnerCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CSpinnerCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CSpinnerCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( UseHostOwnerDraw() && pDC )
	{
		CRect rc;
		GetClientRect( &rc );
		pDC->FillSolidRect( &rc, GetPaneFaceColor() );
		return TRUE;
	}
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CSpinnerCtrl::OnPaint()
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
	PaintHostSpin( &dc );
}
