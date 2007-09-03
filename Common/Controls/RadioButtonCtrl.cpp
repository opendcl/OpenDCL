// RadioButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "RadioButtonCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CRadioButtonCtrl

CRadioButtonCtrl::CRadioButtonCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
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

	return bSuccess;
}

DWORD CRadioButtonCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON);
	return dwStyle;
}

bool CRadioButtonCtrl::OnApplyProperty( TPropertyPtr pProp )
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
			SetCheck( pProp->GetBooleanValue()? BST_CHECKED : BST_UNCHECKED );
		}
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CRadioButtonCtrl, CButton)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRadioButtonCtrl message handlers

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
	mpTemplate->SetBooleanProperty( Prop::Value, (GetCheck() != BST_UNCHECKED) );
}

void CRadioButtonCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	mpTemplate->SetBooleanProperty( Prop::Value, (GetCheck() != BST_UNCHECKED) );
}

HBRUSH CRadioButtonCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if( !IsWindowEnabled() )
		return NULL;
	pDC->SetBkMode( TRANSPARENT );	
	pDC->SetTextColor( mAcadColorService.GetForegroundColor() );
	return mAcadColorService.GetBackgroundBrush();	
}

void CRadioButtonCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
