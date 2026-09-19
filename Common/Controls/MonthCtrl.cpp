// MonthCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MonthCtrl.h"
#include "ControlPane.h"
#include "HostThemeHelper.h"
#include "ColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CMonthCtrl

CMonthCtrl::CMonthCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CMonthCtrl::~CMonthCtrl()
{
}

bool CMonthCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	CRect rcCtrl = GetWndRect();
	bool bSuccess = (__super::Create( GetWndStyle(), rcCtrl, pParentWnd, nID ) != FALSE);
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
	{
		CTime tmToday;
		GetToday( tmToday );
		SetCurSel( tmToday );
		CRect rcMin;
		if( GetMinReqRect( &rcMin ) )
		{
			if( rcMin.Width() > rcCtrl.Width() )
				mpTemplate->SetLongProperty( Prop::Width, rcMin.Width() );
			if( rcMin.Height() > rcCtrl.Height() )
				mpTemplate->SetLongProperty( Prop::Height, rcMin.Height() );
		}
	}

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess )
		SyncHostMonthTheme();

	return bSuccess;
}

DWORD CMonthCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();

	if( mpTemplate->GetLongProperty( Prop::MultiSelection ) > 1 )
		dwStyle |= MCS_MULTISELECT;
	return dwStyle;
}

bool CMonthCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::MultiSelection:
		if( pProp->GetLongValue() > 1 )
			ModifyStyle( 0, MCS_MULTISELECT );
		else
			ModifyStyle( MCS_MULTISELECT, 0 );
		break;
	}
	return !bFailed;
}

void CMonthCtrl::HandleHostThemeChanged()
{
	SyncHostMonthTheme();
}

void CMonthCtrl::SyncHostMonthTheme()
{
	if( !m_hWnd )
		return;
	LPCWSTR pszTheme = CHostThemeHelper::HostMaps()? L"" : NULL;
	GetTheme().SetWindowTheme( pszTheme, pszTheme );
	CHostThemeHelper::ApplyTree( m_hWnd, pszTheme );
	SetColor( MCSC_BACKGROUND, OdclSysColor( COLOR_WINDOW ) );
	SetColor( MCSC_MONTHBK, OdclSysColor( COLOR_WINDOW ) );
	SetColor( MCSC_TEXT, OdclSysColor( COLOR_WINDOWTEXT ) );
	SetColor( MCSC_TITLEBK, OdclSysColor( COLOR_ACTIVECAPTION ) );
	SetColor( MCSC_TITLETEXT, OdclSysColor( COLOR_CAPTIONTEXT ) );
	SetColor( MCSC_TRAILINGTEXT, OdclSysColor( COLOR_GRAYTEXT ) );
	OnNeedRepaint( true );
}


#define WM_ODCL_MONTHNAV (WM_APP + 41)

BEGIN_MESSAGE_MAP(CMonthCtrl, CMonthCalCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_ODCL_MONTHNAV, &CMonthCtrl::OnMonthNavPaint)
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CMonthCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMonthCtrl message handlers

LRESULT CMonthCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

void CMonthCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CMonthCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

BOOL CMonthCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

#ifndef MCGIP_PREV
#define MCGIP_NEXT 1
#define MCGIP_PREV 2
#define MCGIF_RECT 2
#define MCM_GETCALENDARGRIDINFO (MCM_FIRST + 24)
typedef struct tagOdclMCGRIDINFO
{
	UINT cbSize;
	DWORD dwPart;
	DWORD dwFlags;
	int iCalendar;
	int iRow;
	int iCol;
	BOOL bSelected;
	SYSTEMTIME stStart;
	SYSTEMTIME stEnd;
	RECT rc;
	PWSTR pszName;
	size_t cchName;
} OdclMCGRIDINFO;
#else
typedef MCGRIDINFO OdclMCGRIDINFO;
#endif

static bool MonthNavRect( CMonthCalCtrl* pCal, DWORD dwPart, CRect& rc )
{
	OdclMCGRIDINFO gi = {0};
	gi.cbSize = sizeof( gi );
	gi.dwPart = dwPart;
	gi.dwFlags = MCGIF_RECT;
	if( pCal->SendMessage( MCM_GETCALENDARGRIDINFO, 0, (LPARAM)&gi ) )
	{
		rc = gi.rc;
		return !rc.IsRectEmpty();
	}
	return false;
}

static void PaintMonthNavButton( CDC* pDC, const CRect& rc, int nDir )
{
	if( !pDC || rc.Width() < 6 || rc.Height() < 6 )
		return;
	pDC->FillSolidRect( &rc, OdclSysColor( COLOR_BTNFACE ) );
	CHostThemeHelper::PaintRaisedInner( pDC->GetSafeHdc(), rc );
	const int cx = (rc.left + rc.right) / 2;
	const int cy = (rc.top + rc.bottom) / 2;
	const int s = max( 2, min( rc.Width(), rc.Height() ) / 4 );
	POINT pts[3] = {0};
	if( nDir == 0 )
	{
		pts[0].x = cx - s; pts[0].y = cy;
		pts[1].x = cx + s / 2; pts[1].y = cy - s;
		pts[2].x = cx + s / 2; pts[2].y = cy + s;
	}
	else
	{
		pts[0].x = cx + s; pts[0].y = cy;
		pts[1].x = cx - s / 2; pts[1].y = cy - s;
		pts[2].x = cx - s / 2; pts[2].y = cy + s;
	}
	CBrush br( CHostThemeHelper::SoftGlyphColor() );
	CBrush* pOldBrush = pDC->SelectObject( &br );
	CPen* pOldPen = (CPen*)pDC->SelectStockObject( NULL_PEN );
	pDC->Polygon( pts, 3 );
	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldBrush );
}

void CMonthCtrl::OnPaint()
{
	Default();
	if( CHostThemeHelper::HostMaps() )
		PostMessage( WM_ODCL_MONTHNAV );
}

LRESULT CMonthCtrl::OnMonthNavPaint( WPARAM, LPARAM )
{
	if( !CHostThemeHelper::HostMaps() || !m_hWnd )
		return 0;
	CClientDC dc( this );
	CRect rcClient;
	GetClientRect( &rcClient );
	CRect rcPrev;
	CRect rcNext;
	if( MonthNavRect( this, MCGIP_PREV, rcPrev ) )
	{
		if( !rcClient.PtInRect( rcPrev.CenterPoint() ) )
			ScreenToClient( &rcPrev );
	}
	if( MonthNavRect( this, MCGIP_NEXT, rcNext ) )
	{
		if( !rcClient.PtInRect( rcNext.CenterPoint() ) )
			ScreenToClient( &rcNext );
	}
	if( rcPrev.IsRectEmpty() || rcNext.IsRectEmpty() )
	{
		const int nBtn = max( 16, GetSystemMetrics( SM_CXVSCROLL ) );
		rcPrev.SetRect( rcClient.left + 4, rcClient.top + 4, rcClient.left + 4 + nBtn, rcClient.top + 4 + nBtn );
		rcNext.SetRect( rcClient.right - 4 - nBtn, rcClient.top + 4, rcClient.right - 4, rcClient.top + 4 + nBtn );
	}
	PaintMonthNavButton( &dc, rcPrev, 0 );
	PaintMonthNavButton( &dc, rcNext, 1 );
	return 0;
}
