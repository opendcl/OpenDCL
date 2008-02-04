// SlideCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SlideCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CSlideCtrl

CSlideCtrl::CSlideCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CSlideCtrl::~CSlideCtrl()
{
}

bool CSlideCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CSlideCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (TBS_AUTOTICKS);

	if( mpTemplate->GetLongProperty( Prop::Orientation ) == 1 )
		dwStyle |= TBS_VERT;
	else
		dwStyle |= TBS_HORZ;
	
	if( !mpTemplate->GetBooleanProperty( Prop::ShowTicks ) )
		dwStyle |= TBS_NOTICKS;

	return dwStyle;
}

bool CSlideCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Value:
		SetPos( pProp->GetLongValue() );
		break;
	case Prop::MinValue:
		SetRangeMin( pProp->GetLongValue() );
		break;
	case Prop::MaxValue:
		SetRangeMax( pProp->GetLongValue() );
		break;
	case Prop::SmallChange:
		SetLineSize( pProp->GetLongValue() );
		break;
	case Prop::LargeChange:
		SetPageSize( pProp->GetLongValue() );
		break;
	case Prop::ShowTicks:
		if( pProp->GetBooleanValue() )
			ModifyStyle( TBS_NOTICKS, 0 );
		else
			ModifyStyle( 0, TBS_NOTICKS );
		Invalidate();
		break;
	case Prop::TickFrequency:
		SetTicFreq( pProp->GetLongValue() );
		Invalidate();
		break;
	case Prop::Orientation:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( TBS_VERT, TBS_HORZ, SWP_FRAMECHANGED );
		else
			ModifyStyle( TBS_HORZ, TBS_VERT, SWP_FRAMECHANGED );
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CSlideCtrl, CSliderCtrl)
	ON_NOTIFY_REFLECT(NM_RELEASEDCAPTURE, OnReleasedcapture)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlideCtrl message handlers

void CSlideCtrl::OnReleasedcapture(NMHDR* pNMHDR, LRESULT* pResult) 
{
	mpTemplate->SetLongProperty( Prop::Value, GetPos() );
	*pResult = 0;
}

void CSlideCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( nFlags != 0 )
		mpTemplate->SetLongProperty( Prop::Value, GetPos() );
	__super::OnMouseMove(nFlags, point);
}

void CSlideCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	mpTemplate->SetLongProperty( Prop::Value, GetPos() );
	__super::OnLButtonDblClk(nFlags, point);
}

void CSlideCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	mpTemplate->SetLongProperty( Prop::Value, GetPos() );
	__super::OnLButtonDown(nFlags, point);
}

void CSlideCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	mpTemplate->SetLongProperty( Prop::Value, GetPos() );
	__super::OnLButtonUp(nFlags, point);
}

void CSlideCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	mpTemplate->SetLongProperty( Prop::Value, GetPos() );
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSlideCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	mpTemplate->SetLongProperty( Prop::Value, GetPos() );
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CSlideCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CSlideCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CSlideCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if( !IsWindowEnabled() )
		return NULL;
	return mAcadColorService.CtlColor( pDC, nCtlColor );
}
