// ArxAngleSlideCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxAngleSlideCtrl.h"
#include "ControlPane.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxAngleSlideCtrl

CArxAngleSlideCtrl::CArxAngleSlideCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CAngleSlideCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mnLastReportedPosition( 0 )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxAngleSlideCtrl::~CArxAngleSlideCtrl()
{
}

bool CArxAngleSlideCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );
	if( bSuccess )
		mnLastReportedPosition = GetPos();

	return bSuccess;
}

void CArxAngleSlideCtrl::PostMessageToParent(const int nTBCode) const
{
	__super::PostMessageToParent( nTBCode );
	int nNewPos = GetPos();
	if( nNewPos != mnLastReportedPosition )
	{
		const_cast< CArxAngleSlideCtrl* >(this)->mnLastReportedPosition = nNewPos;
		GetArxServices()->HandleEvent( Prop::EventScroll, args_N( nNewPos ) );
	}
}


BEGIN_MESSAGE_MAP(CArxAngleSlideCtrl, CAngleSlideCtrl)
	ON_NOTIFY_REFLECT(NM_OUTOFMEMORY, OnOutofmemory)
	ON_NOTIFY_REFLECT(NM_RELEASEDCAPTURE, OnReleasedcapture)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArxAngleSlideCtrl message handlers

void CArxAngleSlideCtrl::OnOutofmemory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	GetArxServices()->HandleEvent( Prop::EventOutOfMemory );
	*pResult = 0;
}

void CArxAngleSlideCtrl::OnReleasedcapture(NMHDR* pNMHDR, LRESULT* pResult) 
{
	GetArxServices()->HandleEvent( Prop::EventReleasedCapture, args_N( GetPos() ) );
	*pResult = 0;
}

void CArxAngleSlideCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);
	GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) );
}
