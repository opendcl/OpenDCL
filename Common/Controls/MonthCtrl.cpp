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
	if( CHostThemeHelper::HostMaps() )
	{
		SetColor( MCSC_BACKGROUND, OdclSysColor( COLOR_WINDOW ) );
		SetColor( MCSC_MONTHBK, OdclSysColor( COLOR_WINDOW ) );
		SetColor( MCSC_TEXT, OdclSysColor( COLOR_WINDOWTEXT ) );
		SetColor( MCSC_TITLEBK, OdclSysColor( COLOR_ACTIVECAPTION ) );
		SetColor( MCSC_TITLETEXT, OdclSysColor( COLOR_CAPTIONTEXT ) );
		SetColor( MCSC_TRAILINGTEXT, OdclSysColor( COLOR_GRAYTEXT ) );
	}
	OnNeedRepaint( true );
}


BEGIN_MESSAGE_MAP(CMonthCtrl, CMonthCalCtrl)
	ON_WM_ERASEBKGND()
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
