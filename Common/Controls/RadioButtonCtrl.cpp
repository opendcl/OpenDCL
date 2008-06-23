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

CRadioButtonCtrl::CRadioButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
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

	dwStyle |= (/*WS_CLIPSIBLINGS | */BS_AUTORADIOBUTTON | BS_NOTIFY);
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
		SetWindowText( pProp->GetStringValue() );
		break;
	case Prop::Value:
		SetCheck( pProp->GetLongValue() );
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
	mpTemplate->SetLongProperty( Prop::Value, GetCheck() );
}

void CRadioButtonCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	mpTemplate->SetLongProperty( Prop::Value, GetCheck() );
}

HBRUSH CRadioButtonCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbrBackground = mColorService.CtlColor( pDC, nCtlColor, this );
	//if( GetThemeHelper() && mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) )
	//	return NULL; //must use class brush when themes are active, else XP paints a black background
	return hbrBackground;
}

void CRadioButtonCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
