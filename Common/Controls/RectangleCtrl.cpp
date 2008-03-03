// RectangleCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "RectangleCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CRectangleCtrl

CRectangleCtrl::CRectangleCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CRectangleCtrl::~CRectangleCtrl()
{
}

bool CRectangleCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

bool CRectangleCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	//switch( pProp->GetID() )
	//{
	//case Prop::Caption:
	//	{
	//		SetWindowText( pProp->GetStringValue() );
	//	}
	//	break;
	//}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CRectangleCtrl, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRectangleCtrl message handlers

BOOL CRectangleCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CRectangleCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return mColorService.CtlColor( pDC, nCtlColor );
}

void CRectangleCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
