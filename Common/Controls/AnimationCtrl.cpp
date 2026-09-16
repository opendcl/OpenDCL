// AnimationCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AnimationCtrl.h"
#include "ControlPane.h"
#include "HostThemeHelper.h"
#include "ColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CAnimationCtrl

CAnimationCtrl::CAnimationCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CAnimationCtrl::~CAnimationCtrl()
{
}

bool CAnimationCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( bSuccess )
		SyncHostAnimTheme();

	return bSuccess;
}

DWORD CAnimationCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (ACS_CENTER | ACS_AUTOPLAY | ACS_TRANSPARENT);
	return dwStyle;
}


BEGIN_MESSAGE_MAP(CAnimationCtrl, CAnimateCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CAnimationCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAnimationCtrl message handlers

LRESULT CAnimationCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

BOOL CAnimationCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CAnimationCtrl::HandleHostThemeChanged()
{
	SyncHostAnimTheme();
}

void CAnimationCtrl::SyncHostAnimTheme()
{
	if( !m_hWnd )
		return;
	LPCWSTR pszTheme = CHostThemeHelper::HostMaps()? L"" : NULL;
	GetTheme().SetWindowTheme( pszTheme, pszTheme );
	CHostThemeHelper::Apply( m_hWnd, pszTheme );
	OnNeedRepaint( true );
}

HBRUSH CAnimationCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbrBackground = HandleCtlColor( pDC, nCtlColor );
	if( hbrBackground )
		return hbrBackground;
	if( CHostThemeHelper::HostMaps() )
	{
		const COLORREF crFace = OdclSysColor( COLOR_BTNFACE );
		pDC->SetTextColor( OdclSysColor( COLOR_BTNTEXT ) );
		pDC->SetBkColor( crFace );
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
	return NULL;
}

BOOL CAnimationCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( CHostThemeHelper::HostMaps() && pDC )
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

void CAnimationCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
