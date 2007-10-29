// TextButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TextButtonCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CTextButtonCtrl

CTextButtonCtrl::CTextButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CTextButtonCtrl::~CTextButtonCtrl()
{
}

bool CTextButtonCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CTextButtonCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (WS_CLIPSIBLINGS | BS_PUSHBUTTON | BS_MULTILINE);
	return dwStyle;
}

bool CTextButtonCtrl::OnApplyProperty( TPropertyPtr pProp )
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
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CTextButtonCtrl, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_NOTIFY_REFLECT (NM_CUSTOMDRAW, &CTextButtonCtrl::OnNotifyCustomDraw)
	ON_WM_KILLFOCUS()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextButtonCtrl message handlers

BOOL CTextButtonCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CTextButtonCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if( !IsWindowEnabled() )
		return NULL;
	pDC->SetBkMode( TRANSPARENT );	
	pDC->SetTextColor( mAcadColorService.GetForegroundColor() );
	return mAcadColorService.GetBackgroundBrush();	
}

void CTextButtonCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CTextButtonCtrl::OnNotifyCustomDraw ( NMHDR * pNotifyStruct, LRESULT* result )
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

void CTextButtonCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	ModifyStyle( BS_DEFPUSHBUTTON, BS_PUSHBUTTON, SWP_FRAMECHANGED );
}

void CTextButtonCtrl::OnNcPaint() 
{
	if( GetFocus() != this )
		ModifyStyle( BS_DEFPUSHBUTTON, BS_PUSHBUTTON, SWP_FRAMECHANGED );
	else
		ModifyStyle( BS_PUSHBUTTON, BS_DEFPUSHBUTTON, SWP_FRAMECHANGED );
}
