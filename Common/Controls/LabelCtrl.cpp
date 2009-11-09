// LabelCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "LabelCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CLabelCtrl

CLabelCtrl::CLabelCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CLabelCtrl::~CLabelCtrl()
{
}

bool CLabelCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CLabelCtrl::GetWndStyle() const
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

bool CLabelCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
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

void CLabelCtrl::OnValidateBkgnd( CWnd* pBkgnd )
{
	CRect rcClient;
	GetClientRect( &rcClient );
	if( pBkgnd )
	{
		ClientToScreen( &rcClient );
		pBkgnd->ScreenToClient( &rcClient );
		pBkgnd->ValidateRect( &rcClient );
	}
	else
		ValidateRect( &rcClient );
}


BEGIN_MESSAGE_MAP(CLabelCtrl, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLabelCtrl message handlers

BOOL CLabelCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CLabelCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbrBackground = HandleCtlColor( pDC, nCtlColor );
	if( hbrBackground )
		return hbrBackground;
	if( GetThemeHelper() && mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) )
		return NULL; //when using visual style, transparent brush causes class background to be used
	return CAcadColorService::GetTransparentBrush();
}

BOOL CLabelCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CLabelCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
