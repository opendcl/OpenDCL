// ArxSpinnerCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxSpinnerCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxSpinnerCtrl

CArxSpinnerCtrl::CArxSpinnerCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CSpinnerCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxSpinnerCtrl::~CArxSpinnerCtrl()
{
}

bool CArxSpinnerCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	return bSuccess;
}


BEGIN_MESSAGE_MAP(CArxSpinnerCtrl, CSpinnerCtrl)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltapos)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxSpinnerCtrl message handlers

void CArxSpinnerCtrl::OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult)
{
	__super::OnDeltapos( pNMHDR, pResult );
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	GetArxServices()->HandleEvent( Prop::EventChanged,
																 args_NN( mpTemplate->GetLongProperty( Prop::Value ),
																					pNMUpDown->iDelta ) );
}

void CArxSpinnerCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
	__super::OnMouseMove( nFlags, point );
}
