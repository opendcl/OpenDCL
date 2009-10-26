// CheckBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "CheckBoxCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "PropertyIds.h"


//from CommCtrl.h
#define BCN_FIRST               (0U-1250U)
#define BCN_HOTITEMCHANGE       (BCN_FIRST + 0x0001)


/////////////////////////////////////////////////////////////////////////////
// CCheckBoxCtrl

CCheckBoxCtrl::CCheckBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
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

	dwStyle |= (/*WS_CLIPSIBLINGS | */BS_3STATE | BS_NOTIFY);
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
		SetWindowText( pProp->GetStringValue() );
		break;
	case Prop::Value:
		SetCheck( pProp->GetLongValue() );
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CCheckBoxCtrl, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYUP()
	ON_CONTROL_REFLECT(BN_CLICKED, &CCheckBoxCtrl::OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, &CCheckBoxCtrl::OnDoubleclicked)
	ON_NOTIFY_REFLECT(BCN_HOTITEMCHANGE, &CCheckBoxCtrl::OnBnHotItemChange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCheckBoxCtrl message handlers

BOOL CCheckBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	if (pMsg->message == WM_KEYDOWN )
	{
		if( pMsg->wParam == VK_RETURN )
			pMsg->wParam = VK_TAB;
	}
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CCheckBoxCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	//return mColorService.CtlColor( pDC, nCtlColor );
	//return mColorService.CtlColor( pDC, nCtlColor, (mColorService.IsBackgroundTransparent()? this : NULL) );
	HBRUSH hbrBackground = HandleCtlColor( pDC, nCtlColor );
	if( GetThemeHelper() &&
			(LOBYTE(LOWORD(GetVersion())) < 6) &&
			mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) )
		return NULL; //must use class brush when XP themes are active (else XP paints a black background)
	return hbrBackground;
}

BOOL CCheckBoxCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CCheckBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CCheckBoxCtrl::OnClicked() 
{
	int nState = (mpTemplate->GetLongProperty( Prop::Value ) != BST_CHECKED? BST_CHECKED : BST_UNCHECKED);
	mpTemplate->SetLongProperty( Prop::Value, nState );
	SetCheck( nState );
}

void CCheckBoxCtrl::OnDoubleclicked() 
{
	int nState = (mpTemplate->GetLongProperty( Prop::Value ) != BST_CHECKED? BST_CHECKED : BST_UNCHECKED);
	mpTemplate->SetLongProperty( Prop::Value, nState );
	SetCheck( nState );
}

void CCheckBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	OnNeedRepaint(); //must erase background here or focus rectangle gets erased afterward
	__super::OnLButtonDown(nFlags, point);
}

void CCheckBoxCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( IsWindowEnabled() && nChar == _T(' ') )
	{
		GetParent()->PostMessage( WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd );
		return;
	}
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCheckBoxCtrl::OnBnHotItemChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	//OnNeedRepaint();
}
