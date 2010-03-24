// FrameCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FrameCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CFrameCtrl

CFrameCtrl::CFrameCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mColorService( GetSysColor( COLOR_BTNTEXT ), CLR_DEFAULT )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CFrameCtrl::~CFrameCtrl()
{
}

bool CFrameCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess )
		ModifyStyleEx( 0, WS_EX_TRANSPARENT );

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CFrameCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (WS_CLIPSIBLINGS | BS_GROUPBOX);
	return dwStyle;
}


BEGIN_MESSAGE_MAP(CFrameCtrl, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFrameCtrl message handlers

BOOL CFrameCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CFrameCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbrBackground = HandleCtlColor( pDC, nCtlColor );
	if( hbrBackground )
		return hbrBackground;
	return NULL;
	if( GetThemeHelper() && mpTemplate->GetBooleanProperty( Prop::UseVisualStyle ) )
		return NULL; //when using visual style, transparent brush causes class background to be used
	return CAcadColorService::GetTransparentBrush();
}

BOOL CFrameCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return TRUE;
}

void CFrameCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CFrameCtrl::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//lpwndpos->flags |= SWP_NOZORDER;
	__super::OnWindowPosChanging(lpwndpos);
}
