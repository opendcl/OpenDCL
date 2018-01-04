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

void CArxSlideCtrl::OnPositionChanged( int nNewPos, bool bNotify /*= true*/ )
{
	if( nNewPos != mnLastReportedPosition )
	{
		mnLastReportedPosition = nNewPos;
		if( bNotify )
			GetArxServices()->HandleEvent( Prop::EventScroll, args_N( nNewPos ) );
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
	if( GetArxServices()->HandleEvent( Prop::EventOutOfMemory ) )
		return;
	*pResult = 0;
}

void CArxSlideCtrl::OnReleasedcapture(NMHDR* pNMHDR, LRESULT* pResult) 
{
	GetArxServices()->HandleEvent( Prop::EventReleasedCapture, args_N( GetPos() ) );
	*pResult = 0;
}

void CArxSlideCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, ToDIP( point.x ), ToDIP( point.y ) ) );
}
