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
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
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
	HBRUSH hbrBackground = mColorService.CtlColor( pDC, nCtlColor, this );
	//if( GetThemeHelper() && mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) )
	//	return NULL; //must use class brush when themes are active, else XP paints a black background
	return hbrBackground;
}

void CCheckBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CCheckBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
	if( !IsWindowEnabled() )
		return;
	//SetState( GetState() | BST_FOCUS );
	//Invalidate();
}

void CCheckBoxCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( IsWindowEnabled() )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		if( rcClient.PtInRect( point ) )
		{
			int nState = (mpTemplate->GetLongProperty( Prop::Value ) != BST_CHECKED? BST_CHECKED : BST_UNCHECKED);
			mpTemplate->SetLongProperty( Prop::Value, nState );
			SetCheck( nState );
			Invalidate();
		}
	}
	__super::OnLButtonUp(nFlags, point);
}

void CCheckBoxCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( IsWindowEnabled() && nChar == _T(' ') )
	{
		int nState = (mpTemplate->GetLongProperty( Prop::Value ) != BST_CHECKED? BST_CHECKED : BST_UNCHECKED);
		mpTemplate->SetLongProperty( Prop::Value, nState );
		SetCheck( nState );
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd );
		Invalidate();
		return;
	}
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCheckBoxCtrl::OnKillFocus(CWnd * pNewWnd)
{
	//SetState( GetState() & ~BST_FOCUS );
	//Invalidate();
	__super::OnKillFocus( pNewWnd );
}
