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
	if( bSuccess )
	{
	#ifdef _UNICODE
		BOOL bUnicode = TRUE;
	#else
		BOOL bUnicode = FALSE;
	#endif
		SendMessage( CCM_SETUNICODEFORMAT, (WPARAM)bUnicode, 0 );
	}

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
		OnNeedRepaint();
		break;
	case Prop::TickFrequency:
		SetTicFreq( pProp->GetLongValue() );
		OnNeedRepaint();
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

bool CSlideCtrl::OnApplyBackgroundColor( TPropertyPtr pProp )
{
	bool bSuccess = __super::OnApplyBackgroundColor( pProp );
	ClearSel( TRUE ); //force it to repaint its background
	return bSuccess;
}


BEGIN_MESSAGE_MAP(CSlideCtrl, CSliderCtrl)
	ON_WM_HSCROLL_REFLECT()
	ON_WM_VSCROLL_REFLECT()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlideCtrl message handlers

void CSlideCtrl::HScroll(UINT nSBCode, UINT nPos)
{
	int nNewPos = GetPos();
	mpTemplate->SetLongProperty( Prop::Value, nNewPos );
	OnPositionChanged( nNewPos );
}

void CSlideCtrl::VScroll(UINT nSBCode, UINT nPos)
{
	int nNewPos = GetPos();
	mpTemplate->SetLongProperty( Prop::Value, nNewPos );
	OnPositionChanged( nNewPos );
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
	return mColorService.CtlColor( pDC, nCtlColor, (mColorService.IsBackgroundTransparent()? this : NULL) );
}

BOOL CSlideCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( mColorService.IsBackgroundTransparent() )
		return TRUE;

	return __super::OnEraseBkgnd(pDC);
}
