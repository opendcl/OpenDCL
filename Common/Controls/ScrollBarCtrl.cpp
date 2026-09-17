// ScrollBarCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ScrollBarCtrl.h"
#include "ControlPane.h"
#include "HostThemeHelper.h"
#include "ColorService.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CScrollBarCtrl

CScrollBarCtrl::CScrollBarCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CScrollBarCtrl::~CScrollBarCtrl()
{
}

bool CScrollBarCtrl::Create( CWnd* pParentWnd, UINT nID ) 
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

	if( bSuccess )
		ShowScrollBar();

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess )
	{
		ApplyScrollInfo();
		SyncHostScrollTheme();
	}

	return bSuccess;
}

DWORD CScrollBarCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (SBS_BOTTOMALIGN);

	if( mpTemplate->GetLongProperty( Prop::Orientation ) == 0 )
		dwStyle |= SBS_HORZ;
	else
		dwStyle |= SBS_VERT;

	return dwStyle;
}

bool CScrollBarCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Value:
	case Prop::MinValue:
	case Prop::MaxValue:
	case Prop::LargeChange:
		if( !IsEnumeratingProperties() )
			ApplyScrollInfo();
		break;
	case Prop::Orientation:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( SBS_VERT, SBS_HORZ, SWP_FRAMECHANGED );
		else
			ModifyStyle( SBS_HORZ, SBS_VERT, SWP_FRAMECHANGED );
		break;
	case Prop::SmallChange:
		break;
	}
	return !bFailed;
}

void CScrollBarCtrl::ApplyScrollInfo()
{
	if( !m_hWnd || !mpTemplate )
		return;
	SCROLLINFO si = {0};
	si.cbSize = sizeof( si );
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMin = mpTemplate->GetLongProperty( Prop::MinValue );
	si.nMax = mpTemplate->GetLongProperty( Prop::MaxValue );
	const UINT nPage = (UINT)max( 1L, mpTemplate->GetLongProperty( Prop::LargeChange ) );
	si.nPage = nPage;
	si.nPos = mpTemplate->GetLongProperty( Prop::Value );
	SetScrollInfo( &si, TRUE );
}

bool CScrollBarCtrl::UseHostOwnerDraw() const
{
	// Always owner-draw. Native UxTheme (especially Win11 overlay) is light
	// on CAD chrome and does not consume OdclSysColor.
	return true;
}

void CScrollBarCtrl::HandleHostThemeChanged()
{
	SyncHostScrollTheme();
}

bool CScrollBarCtrl::OnApplyUseVisualStyle( TPropertyPtr pProp )
{
	if( !__super::OnApplyUseVisualStyle( pProp ) )
		return false;
	SyncHostScrollTheme();
	return true;
}

void CScrollBarCtrl::SyncHostScrollTheme()
{
	if( !m_hWnd )
		return;
	GetTheme().SetWindowTheme( L"", L"" );
	CHostThemeHelper::Apply( m_hWnd, L"" );
	OnNeedRepaint( true );
}

static void DrawScrollArrow( CDC* pDC, const CRect& rc, int nDir )
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

void CScrollBarCtrl::PaintHostScrollBar( CDC* pDC )
{
	if( !pDC )
		return;
	CRect rc;
	GetClientRect( &rc );
	const bool bVert = (GetStyle() & SBS_VERT) != 0;
	const int nArrow = bVert? GetSystemMetrics( SM_CYVSCROLL ) : GetSystemMetrics( SM_CXHSCROLL );

	CRect rcArrow1 = rc;
	CRect rcArrow2 = rc;
	CRect rcTrack = rc;
	if( bVert )
	{
		rcArrow1.bottom = rc.top + nArrow;
		rcArrow2.top = rc.bottom - nArrow;
		rcTrack.top = rcArrow1.bottom;
		rcTrack.bottom = rcArrow2.top;
	}
	else
	{
		rcArrow1.right = rc.left + nArrow;
		rcArrow2.left = rc.right - nArrow;
		rcTrack.left = rcArrow1.right;
		rcTrack.right = rcArrow2.left;
	}

	pDC->FillSolidRect( &rcTrack, OdclSysColor( COLOR_3DSHADOW ) );
	DrawScrollArrow( pDC, rcArrow1, bVert? 0 : 2 );
	DrawScrollArrow( pDC, rcArrow2, bVert? 1 : 3 );

	SCROLLINFO si = {0};
	si.cbSize = sizeof( si );
	si.fMask = SIF_ALL;
	GetScrollInfo( &si );
	const int nRange = si.nMax - si.nMin + 1;
	const int nPage = (int)max( (UINT)1, si.nPage );
	const int nTrack = bVert? rcTrack.Height() : rcTrack.Width();
	if( nRange > nPage && nTrack > 0 )
	{
		int nThumb = max( nArrow, MulDiv( nPage, nTrack, nRange ) );
		if( nThumb > nTrack )
			nThumb = nTrack;
		const int nTravel = nTrack - nThumb;
		const int nMaxPos = max( 1, nRange - nPage );
		const int nOffset = MulDiv( si.nPos - si.nMin, nTravel, nMaxPos );
		CRect rcThumb = rcTrack;
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
		pDC->FillSolidRect( &rcThumb, OdclSysColor( COLOR_BTNFACE ) );
		CPen pen( PS_SOLID, 1, OdclSysColor( COLOR_3DLIGHT ) );
		CPen* pOldPen = pDC->SelectObject( &pen );
		CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject( NULL_BRUSH );
		pDC->Rectangle( rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom );
		pDC->SelectObject( pOldBrush );
		pDC->SelectObject( pOldPen );
	}
}

bool CScrollBarCtrl::OnApplyForegroundColor( TPropertyPtr pProp )
{
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
	{
		pColorService->SetForegroundColor( pProp->GetLongValue() );
		pColorService->SetBackgroundColor( pProp->GetLongValue() );
	}
	OnNeedRepaint();
	return true;
}

void CScrollBarCtrl::OnScroll(UINT nSBCode, UINT nPos) 
{
	bool bDoneScrolling = false;
	int nMinPos;
	int nMaxPos;
	GetScrollRange( &nMinPos, &nMaxPos );
	int nNewPos = mpTemplate->GetLongProperty( Prop::Value );
	
	switch( nSBCode )
	{
	case 0: // lesser or min arrow click
		nNewPos -= mpTemplate->GetLongProperty( Prop::SmallChange );
		if( nNewPos < nMinPos )
			nNewPos = nMinPos;
		break;
	case 1: // greater or max arrow click
		nNewPos += mpTemplate->GetLongProperty( Prop::SmallChange );
		if( nNewPos > nMaxPos )
			nNewPos = nMaxPos;
		break;
	case 2: // user has clicked scroll area left/above of Scroll Button indicator
		nNewPos -= mpTemplate->GetLongProperty( Prop::LargeChange );
		if( nNewPos < nMinPos )
			nNewPos = nMinPos;
		break;
	case 3: // user has click scroll area right/below of Scroll Button indicator
		nNewPos += mpTemplate->GetLongProperty( Prop::LargeChange );
		if( nNewPos > nMaxPos )
			nNewPos = nMaxPos;
		break;
	case 5: // user is scrolling the scrollbar
		nNewPos = nPos;
		break;
	case 4: // user is done scrolling the scrollbar
	case 8: // user is done scrolling the scrollbar
		bDoneScrolling = true;
		break;
	default:
		break;
	}
	
	TPropertyPtr pValueProp = mpTemplate->GetPropertyObject( Prop::Value );
	pValueProp->SetLongValue( nNewPos );
	ApplyProperty( pValueProp );
}


BEGIN_MESSAGE_MAP(CScrollBarCtrl, CScrollBar)
	ON_WM_HSCROLL_REFLECT()
	ON_WM_VSCROLL_REFLECT()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CScrollBarCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CScrollBarCtrl message handlers

LRESULT CScrollBarCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

void CScrollBarCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus(pOldWnd);
	if( pOldWnd )
		pOldWnd->SetFocus();
}


void CScrollBarCtrl::HScroll(UINT nSBCode, UINT nPos) 
{
	OnScroll(nSBCode, nPos);
}


void CScrollBarCtrl::VScroll(UINT nSBCode, UINT nPos) 
{
	OnScroll(nSBCode, nPos);
}

void CScrollBarCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CScrollBarCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CScrollBarCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CScrollBarCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( UseHostOwnerDraw() && pDC )
	{
		CRect rc;
		GetClientRect( &rc );
		pDC->FillSolidRect( &rc, OdclSysColor( COLOR_3DSHADOW ) );
		return TRUE;
	}
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CScrollBarCtrl::OnPaint()
{
	if( !UseHostOwnerDraw() )
	{
		Default();
		return;
	}
	CPaintDC dc( this );
	PaintHostScrollBar( &dc );
}
