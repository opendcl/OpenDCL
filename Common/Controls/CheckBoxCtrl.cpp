// CheckBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CheckBoxCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CCheckBoxCtrl

CCheckBoxCtrl::CCheckBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CCheckBoxCtrl::~CCheckBoxCtrl()
{
}

bool CCheckBoxCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CCheckBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (WS_CLIPSIBLINGS | BS_AUTOCHECKBOX);
	return dwStyle;
}

bool CCheckBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Caption:
		{
			SetWindowText( pProp->GetStringValue() );
		}
		break;
	case Prop::Value:
		{
			SetCheck( pProp->GetLongValue() );
		}
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CCheckBoxCtrl, CButton)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCheckBoxCtrl message handlers

BOOL CCheckBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
		pMsg->wParam = VK_TAB;		
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CCheckBoxCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if( !IsWindowEnabled() )
		return NULL;
	pDC->SetBkMode( TRANSPARENT );	
	pDC->SetTextColor( mAcadColorService.GetForegroundColor() );
	return mAcadColorService.GetBackgroundBrush();	
}

void CCheckBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
