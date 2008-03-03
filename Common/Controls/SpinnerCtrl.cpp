// SpinnerCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SpinnerCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CSpinnerCtrl

CSpinnerCtrl::CSpinnerCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CSpinnerCtrl::~CSpinnerCtrl()
{
}

bool CSpinnerCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CSpinnerCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (UDS_ARROWKEYS);

	if( mpTemplate->GetLongProperty( Prop::Orientation ) == 0 )
		dwStyle |= UDS_HORZ;

	if( mpTemplate->GetBooleanProperty( Prop::AutoWrap ) )
		dwStyle |= UDS_WRAP;

	return dwStyle;
}

bool CSpinnerCtrl::OnApplyProperty( TPropertyPtr pProp )
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
		SetRange( (short)pProp->GetLongValue(), (short)mpTemplate->GetLongProperty( Prop::MaxValue ) );
		break;
	case Prop::MaxValue:
		if( !IsEnumeratingProperties() )
			SetRange( (short)mpTemplate->GetLongProperty( Prop::MinValue ), (short)pProp->GetLongValue() );
		break;
	case Prop::Orientation:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( 0, UDS_HORZ, SWP_FRAMECHANGED );
		else
			ModifyStyle( UDS_HORZ, 0, SWP_FRAMECHANGED );
		break;
	case Prop::AutoWrap:
		if( pProp->GetBooleanValue() )
			ModifyStyle( 0, UDS_WRAP );
		else
			ModifyStyle( UDS_WRAP, 0 );
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CSpinnerCtrl, CSpinButtonCtrl)
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltapos)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpinnerCtrl message handlers

void CSpinnerCtrl::OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nMinPos = mpTemplate->GetLongProperty( Prop::MinValue );
	int nMaxPos = mpTemplate->GetLongProperty( Prop::MaxValue );
	int nNewPos = mpTemplate->GetLongProperty( Prop::Value );

	nNewPos -= pNMUpDown->iDelta;
	if( mpTemplate->GetBooleanProperty( Prop::AutoWrap ) )
	{	
		if( nNewPos < nMinPos )
			nNewPos = nMaxPos;
		else if( nNewPos > nMaxPos )
			nNewPos = nMinPos;				
	}
	else
	{	
		if( nNewPos < nMinPos )
			nNewPos = nMinPos;
		else if( nNewPos > nMaxPos )
			nNewPos = nMaxPos;				
	}

	TPropertyPtr pValueProp = mpTemplate->GetPropertyObject( Prop::Value );
	pValueProp->SetLongValue( nNewPos );
	OnApplyProperty( pValueProp );

	*pResult = 0;
}

void CSpinnerCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CSpinnerCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CSpinnerCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return mColorService.CtlColor( pDC, nCtlColor, this );
}
