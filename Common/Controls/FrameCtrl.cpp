// FrameCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FrameCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CFrameCtrl

CFrameCtrl::CFrameCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
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

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CFrameCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (/*WS_CLIPSIBLINGS | */BS_PUSHBUTTON | BS_GROUPBOX);
	return dwStyle;
}

bool CFrameCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	//switch( pProp->GetID() )
	//{
	//case Prop::Caption:
	//	{
	//		SetWindowText( pProp->GetStringValue() );
	//	}
	//	break;
	//}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CFrameCtrl, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_NOTIFY_REFLECT (NM_CUSTOMDRAW, &CFrameCtrl::OnNotifyCustomDraw)
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
	if( !IsWindowEnabled() )
		return NULL;
	return mAcadColorService.CtlColor( pDC, nCtlColor );
}

void CFrameCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CFrameCtrl::OnNotifyCustomDraw ( NMHDR * pNotifyStruct, LRESULT* result )
{
	LPNMCUSTOMDRAW pCustomDraw = (LPNMCUSTOMDRAW)pNotifyStruct;
	ASSERT (pCustomDraw->hdr.hwndFrom == m_hWnd);
	ASSERT (pCustomDraw->hdr.code = NM_CUSTOMDRAW);

	if( pCustomDraw->dwDrawStage == CDDS_PREPAINT )
	{
		::SetTextColor( pCustomDraw->hdc, mAcadColorService.GetForegroundColor() );
		::SetBkColor( pCustomDraw->hdc, mAcadColorService.GetBackgroundColor() );
	}
	*result = CDRF_DODEFAULT;
}
