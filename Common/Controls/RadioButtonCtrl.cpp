// RadioButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "RadioButtonCtrl.h"
#include "DclControlTemplate.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "PropertyIds.h"


//from CommCtrl.h
#define BCN_FIRST               (0U-1250U)
#define BCN_HOTITEMCHANGE       (BCN_FIRST + 0x0001)


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

bool CRadioButtonCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
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
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(BCN_HOTITEMCHANGE, &CRadioButtonCtrl::OnBnHotItemChange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRadioButtonCtrl message handlers

LRESULT CRadioButtonCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = __super::WindowProc(message, wParam, lParam);
	if( message == BM_SETCHECK )
		mpTemplate->SetLongProperty( Prop::Value, wParam );
	return lResult;
}

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
	OnNeedRepaint();
}

void CRadioButtonCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	OnNeedRepaint();
	__super::OnKillFocus(pNewWnd);
}

HBRUSH CRadioButtonCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbrBackground = HandleCtlColor( pDC, nCtlColor );
	if( hbrBackground )
		return hbrBackground;
	if( GetTheme().IsThemeActive() )
		return NULL; //when using visual style, transparent brush causes class background to be used
	return CAcadColorService::GetTransparentBrush();
}

BOOL CRadioButtonCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CRadioButtonCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CRadioButtonCtrl::OnBnHotItemChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	//OnNeedRepaint();
}
