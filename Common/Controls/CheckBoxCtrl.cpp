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
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
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
	return mColorService.CtlColor( pDC, nCtlColor );
	//return mColorService.CtlColor( pDC, nCtlColor, (mColorService.IsBackgroundTransparent()? this : NULL) );
	//HBRUSH hbrBackground = mColorService.CtlColor( pDC, nCtlColor, this );
	//if( GetThemeHelper() && mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) )
	//	return NULL; //must use class brush when themes are active, else XP paints a black background
	//return hbrBackground;
}

BOOL CCheckBoxCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( mColorService.IsBackgroundTransparent() )
	{
		CRect rcClip;
		pDC->GetClipBox( &rcClip );
		ClientToScreen( &rcClip );
		CWnd* pParentWnd = GetParent();
		pParentWnd->ScreenToClient( &rcClip );
		pParentWnd->RedrawWindow( &rcClip, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_NOCHILDREN | RDW_UPDATENOW );
		return TRUE;
	}

	return __super::OnEraseBkgnd(pDC);
}

void CCheckBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CCheckBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	OnNeedRepaint(); //must erase background here or focus rectangle gets erased afterward
	__super::OnLButtonDown(nFlags, point);
	if( !IsWindowEnabled() )
		return;
	//OnNeedRepaint( false, true );
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
		}
	}
	__super::OnLButtonUp(nFlags, point);
	////OnNeedRepaint();
}

void CCheckBoxCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( IsWindowEnabled() && nChar == _T(' ') )
	{
		int nState = (mpTemplate->GetLongProperty( Prop::Value ) != BST_CHECKED? BST_CHECKED : BST_UNCHECKED);
		mpTemplate->SetLongProperty( Prop::Value, nState );
		SetCheck( nState );
		GetParent()->PostMessage( WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd );
		return;
	}
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCheckBoxCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus(pOldWnd);
}

void CCheckBoxCtrl::OnKillFocus(CWnd * pNewWnd)
{
	//SetState( GetState() & ~BST_FOCUS );
	//Invalidate();
	//OnNeedRepaint();
	__super::OnKillFocus( pNewWnd );
}

void CCheckBoxCtrl::OnBnHotItemChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	OnNeedRepaint();
}
