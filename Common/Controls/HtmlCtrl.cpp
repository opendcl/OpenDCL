// HtmlCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "HtmlCtrl.h" 
#include "ControlPane.h"
#include "Workspace.h"
#include "SharedRes.Local.h"


/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl

CHtmlCtrl::CHtmlCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CHtmlCtrl::~CHtmlCtrl()
{
}

bool CHtmlCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess )
		SetSilent( FALSE );

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CHtmlCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();

	switch( mpTemplate->GetLongProperty( Prop::Justification ) )
	{
	case 0:
		dwStyle |= SS_LEFT;
		break;
	case 1:
		dwStyle |= SS_CENTER;
		break;
	case 2:
		dwStyle |= SS_RIGHT;
		break;
	}
	return dwStyle;
}

void CHtmlCtrl::HandleDpiChanged()
{
	__super::HandleDpiChanged();
	SetOpticalZoom( FromDIP( 100 ) );
}

bool CHtmlCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Justification:
		{
			switch( pProp->GetLongValue() )
			{
			case 0:
				ModifyStyle( (SS_CENTER | SS_RIGHT), SS_LEFT, 0 );
				break;
			case 1:
				ModifyStyle( (SS_LEFT | SS_RIGHT), SS_CENTER, 0 );
				break;
			case 2:
				ModifyStyle( (SS_LEFT | SS_CENTER), SS_RIGHT, 0 );
				break;
			}
			OnNeedRepaint();
		}
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CHtmlCtrl, CHtmlBrowser)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CHtmlCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl message handlers

LRESULT CHtmlCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

HRESULT CHtmlCtrl::OnGetHostInfo(DOCHOSTUIINFO *pInfo)
{
	HRESULT hr = __super::OnGetHostInfo( pInfo );
	if( FAILED(hr) )
		return hr;
	if( pInfo )
		pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DOUTERBORDER;
	return S_OK;
}
void CHtmlCtrl::OnDocumentComplete(LPCTSTR lpszURL)
{
	__super::OnDocumentComplete(lpszURL);
	SetOpticalZoom( FromDIP( 100 ) );
}

BOOL CHtmlCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CHtmlCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CHtmlCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CHtmlCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
