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

	return bSuccess;
}


BEGIN_MESSAGE_MAP(CArxAngleSlideCtrl, CAngleSlideCtrl)
	ON_NOTIFY_REFLECT(NM_OUTOFMEMORY, OnOutofmemory)
	ON_NOTIFY_REFLECT(NM_RELEASEDCAPTURE, OnReleasedcapture)
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArxAngleSlideCtrl message handlers

void CArxAngleSlideCtrl::OnOutofmemory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventOutOfMemory ), IsAsyncEvents() );
	*pResult = 0;
}

void CArxAngleSlideCtrl::OnReleasedcapture(NMHDR* pNMHDR, LRESULT* pResult) 
{
	InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventReleasedCapture ), GetPos(), IsAsyncEvents() );
	*pResult = 0;
}

void CArxAngleSlideCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	__super::OnMouseMove(nFlags, point);
	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents());
}

void CArxAngleSlideCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
	InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventScroll ), nPos, IsAsyncEvents() );
}
