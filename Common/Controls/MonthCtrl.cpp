// MonthCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MonthCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CMonthCtrl

CMonthCtrl::CMonthCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CMonthCtrl::~CMonthCtrl()
{
}

bool CMonthCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	CRect rcCtrl = GetWndRect();
	bool bSuccess = (__super::Create( GetWndStyle(), rcCtrl, pParentWnd, nID ) != FALSE);
	if( bSuccess )
	{
	#ifdef _UNICODE
		BOOL bUnicode = TRUE;
	#else
		BOOL bUnicode = FALSE;
	#endif
		SendMessage( CCM_SETUNICODEFORMAT, (WPARAM)bUnicode, 0 );
	}

	if( bSuccess )
	{
		CTime tmToday;
		GetToday( tmToday );
		SetCurSel( tmToday );
		CRect rcMin;
		if( GetMinReqRect( &rcMin ) )
		{
			if( rcMin.Width() > rcCtrl.Width() )
				mpTemplate->SetLongProperty( Prop::Width, rcMin.Width() );
			if( rcMin.Height() > rcCtrl.Height() )
				mpTemplate->SetLongProperty( Prop::Height, rcMin.Height() );
		}
	}

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CMonthCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();

	if( mpTemplate->GetLongProperty( Prop::MultiSelection ) > 1 )
		dwStyle |= MCS_MULTISELECT;
	return dwStyle;
}

bool CMonthCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::MultiSelection:
		if( pProp->GetLongValue() > 1 )
			ModifyStyle( 0, MCS_MULTISELECT );
		else
			ModifyStyle( MCS_MULTISELECT, 0 );
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CMonthCtrl, CMonthCalCtrl)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMonthCtrl message handlers

void CMonthCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CMonthCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

BOOL CMonthCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}
