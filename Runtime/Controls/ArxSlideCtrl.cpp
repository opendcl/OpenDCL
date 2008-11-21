// ArxSlideCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxSlideCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxSlideCtrl

CArxSlideCtrl::CArxSlideCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CSlideCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mnLastReportedPosition( 0 )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxSlideCtrl::~CArxSlideCtrl()
{
}

bool CArxSlideCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );
	if( bSuccess )
		mnLastReportedPosition = GetPos();

	return bSuccess;
}

void CArxSlideCtrl::OnPositionChanged( int nNewPos )
{
	if( nNewPos != mnLastReportedPosition )
	{
		mnLastReportedPosition = nNewPos;
		InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventScroll ), nNewPos, IsAsyncEvents() );
	}
}


BEGIN_MESSAGE_MAP(CArxSlideCtrl, CSlideCtrl)
	ON_NOTIFY_REFLECT(NM_OUTOFMEMORY, OnOutofmemory)
	ON_NOTIFY_REFLECT(NM_RELEASEDCAPTURE, OnReleasedcapture)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArxSlideCtrl message handlers

void CArxSlideCtrl::OnOutofmemory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventOutOfMemory ), IsAsyncEvents() );
	*pResult = 0;
}

void CArxSlideCtrl::OnReleasedcapture(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventReleasedCapture ), GetPos(), IsAsyncEvents() );
	*pResult = 0;
}

void CArxSlideCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents());

}
