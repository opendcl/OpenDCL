// AngleSlideCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AngleSlideCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CAngleSlideCtrl

CAngleSlideCtrl::CAngleSlideCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CAngleSlideCtrl::~CAngleSlideCtrl()
{
}

bool CAngleSlideCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess )
	{
		SetRange( 0, 359, FALSE );
		SetZero( 270 );
		SetInverted();
	}

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CAngleSlideCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	return dwStyle;
}

bool CAngleSlideCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Value:
		SetPos( pProp->GetLongValue() );
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CAngleSlideCtrl, CRoundSliderCtrl)
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAngleSlideCtrl message handlers

void CAngleSlideCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	mpTemplate->SetLongProperty( Prop::Value, GetPos() );
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAngleSlideCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CAngleSlideCtrl::PreTranslateMessage(MSG* pMsg)
{
	GetToolTipCtrl().RelayEvent( pMsg );
	return __super::PreTranslateMessage( pMsg );
}

HBRUSH CAngleSlideCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
	if( !IsWindowEnabled() )
		return NULL;
	return mAcadColorService.CtlColor( pDC, nCtlColor );
}

void CAngleSlideCtrl::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	CRect rcClient;
	GetClientRect( &rcClient );
	CDC* pDC = GetDC();
	pDC->FillRect( &rcClient, mAcadColorService.GetBackgroundCBrush() );
	ReleaseDC( pDC );
	__super::OnPaint();
	// TODO: Add your message handler code here
	// Do not call __super::OnPaint() for painting messages
}
