// DynamicButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DynamicButtonCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CDynamicButtonCtrl

CDynamicButtonCtrl::CDynamicButtonCtrl( CWnd* pParentWnd, const CRect& rectWnd, UINT nID )
: CXPStyleButtonST()
, mpParentWnd( pParentWnd )
{
	Create( NULL, WS_CHILD | WS_VISIBLE, rectWnd, pParentWnd, nID );
}

CDynamicButtonCtrl::~CDynamicButtonCtrl()
{
	if( m_hWnd )
		DestroyWindow();
}

BEGIN_MESSAGE_MAP(CDynamicButtonCtrl, CXPStyleButtonST)
	//ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
END_MESSAGE_MAP()


void CDynamicButtonCtrl::OnClicked() 
{
	__super::OnClicked();
	if( mpParentWnd )
		mpParentWnd->SendMessage(WM_COMMAND, (WPARAM)GetDlgCtrlID(), (LPARAM)m_hWnd );
}
