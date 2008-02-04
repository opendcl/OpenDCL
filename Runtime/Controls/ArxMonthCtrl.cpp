// ArxMonthCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxMonthCtrl.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxMonthCtrl

CArxMonthCtrl::CArxMonthCtrl( TDclControlPtr pTemplate,
															CControlPane* pPane,
															UINT nID,
															bool bCreate /*= true*/ )
: CMonthCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxMonthCtrl::~CArxMonthCtrl()
{
}


BEGIN_MESSAGE_MAP(CArxMonthCtrl, CMonthCtrl)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(MCN_GETDAYSTATE, OnGetdaystate)
	ON_NOTIFY_REFLECT(MCN_SELCHANGE, OnSelchange)
	ON_NOTIFY_REFLECT(MCN_SELECT, OnSelect)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxMonthCtrl message handlers

void CArxMonthCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents() );
	__super::OnMouseMove( nFlags, point );
}


void CArxMonthCtrl::OnGetdaystate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventGetDayState ), IsAsyncEvents() );
	*pResult = 0;
}

void CArxMonthCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventSelChanged ), IsAsyncEvents() );
	*pResult = 0;
}

void CArxMonthCtrl::OnSelect(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventSelect ), IsAsyncEvents() );
	*pResult = 0;
}
