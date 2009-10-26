// AnimationCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AnimationCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CAnimationCtrl

CAnimationCtrl::CAnimationCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CAnimationCtrl::~CAnimationCtrl()
{
}

bool CAnimationCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CAnimationCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (ACS_CENTER | ACS_AUTOPLAY | ACS_TRANSPARENT);
	return dwStyle;
}


BEGIN_MESSAGE_MAP(CAnimationCtrl, CAnimateCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAnimationCtrl message handlers

BOOL CAnimationCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CAnimationCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbrBackground = HandleCtlColor( pDC, nCtlColor );
	return NULL;
	//return mColorService.GetTransparentBrush();
}

BOOL CAnimationCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CAnimationCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
