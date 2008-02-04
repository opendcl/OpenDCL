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

	return bSuccess;
}


BEGIN_MESSAGE_MAP(CArxSlideCtrl, CSlideCtrl)
	ON_NOTIFY_REFLECT(NM_OUTOFMEMORY, OnOutofmemory)
	ON_NOTIFY_REFLECT(NM_RELEASEDCAPTURE, OnReleasedcapture)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
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

	// if the mouse is down, send the scroll event to AutoLISP
	if( nFlags != 0 )
		InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventScroll ), GetPos(), IsAsyncEvents() );
}

void CArxSlideCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDblClk(nFlags, point);
	InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventScroll ), GetPos(), IsAsyncEvents() );
}

void CArxSlideCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown(nFlags, point);
	InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventScroll ), GetPos(), IsAsyncEvents() );
}

void CArxSlideCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	__super::OnLButtonUp(nFlags, point);
	InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventScroll ), GetPos(), IsAsyncEvents() );
}

void CArxSlideCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
	InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventScroll ), GetPos(), IsAsyncEvents() );
}

void CArxSlideCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
	InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventScroll ), GetPos(), IsAsyncEvents() );
}
