// ArxPictureBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxPictureBoxCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "Resource.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxPictureBoxCtrl

CArxPictureBoxCtrl::CArxPictureBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CPictureBoxCtrl( pTemplate, pPane, nID, false )
, mArxServices( this )
, mDragDropService( this )
, mbTrackingMouse( false )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxPictureBoxCtrl::~CArxPictureBoxCtrl()
{
}


BEGIN_MESSAGE_MAP(CArxPictureBoxCtrl, CPictureBoxCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)   
	ON_WM_LBUTTONDBLCLK()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_WM_CHAR()
	ON_WM_PAINT()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxPictureBoxCtrl message handlers

void CArxPictureBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( nFlags == MK_LBUTTON && mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) )
	{
		DROPEFFECT dwDropEffect = BeginDragDrop( point );
		if( dwDropEffect != DROPEFFECT_NONE )
			return;
	}

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventLMouse),
		0,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	__super::OnLButtonDown( nFlags, point );
}

void CArxPictureBoxCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), IsAsyncEvents());
}

void CArxPictureBoxCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus(pOldWnd);
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), IsAsyncEvents());
}

void CArxPictureBoxCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventLMouse),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	__super::OnLButtonUp(nFlags, point);
}

void CArxPictureBoxCtrl::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMMouse),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
		
	__super::OnMButtonDblClk(nFlags, point);
}

void CArxPictureBoxCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMMouse),
		0,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	__super::OnMButtonDown(nFlags, point);
}

void CArxPictureBoxCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		4,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMMouse),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	__super::OnMButtonUp(nFlags, point);
}

void CArxPictureBoxCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( !mbTrackingMouse )
	{
		TRACKMOUSEEVENT tm = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
		if( _TrackMouseEvent( &tm ) )
			mbTrackingMouse = true;
		InvokeMethod( mpTemplate->GetStringProperty( Prop::EventMouseEntered ), IsAsyncEvents() );
	}

	InvokeMethodIntIntInt( mpTemplate->GetStringProperty( Prop::EventMouseMove ),
												 nFlags,
												 point.x,
												 point.y,
												 IsAsyncEvents() );

	__super::OnMouseMove(nFlags, point);
}

LRESULT CArxPictureBoxCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	mbTrackingMouse = false;        
	InvokeMethod( mpTemplate->GetStringProperty( Prop::EventMouseMovedOff ), IsAsyncEvents() );
	return FALSE;
}

BOOL CArxPictureBoxCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		nFlags,
		zDelta,
		pt.x,
		pt.y,
		IsAsyncEvents());

	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CArxPictureBoxCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	InvokeMethod(
		mpTemplate->GetStringProperty(Prop::EventRightDblClick),
		IsAsyncEvents());

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventRMouse),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	__super::OnRButtonDblClk(nFlags, point);
}

void CArxPictureBoxCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventRMouse),
		0,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	__super::OnRButtonDown(nFlags, point);
}

void CArxPictureBoxCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseUp),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	InvokeMethod(
		mpTemplate->GetStringProperty(Prop::EventRightClick),
		IsAsyncEvents());	

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventRMouse),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	__super::OnRButtonUp(nFlags, point);
}

void CArxPictureBoxCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyDown), sChar, nRepCnt, nFlags, IsAsyncEvents());
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxPictureBoxCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyUp), sChar, nRepCnt, nFlags, IsAsyncEvents());
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxPictureBoxCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDblClick),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	InvokeMethod(
		mpTemplate->GetStringProperty(Prop::EventDblClicked),
		IsAsyncEvents());

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventLMouse),
		2,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	__super::OnLButtonDblClk(nFlags, point);
}

void CArxPictureBoxCtrl::OnClicked() 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventClicked), IsAsyncEvents());
}

void CArxPictureBoxCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == 13 || nChar == 10 )
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventClicked), IsAsyncEvents());
}

void CArxPictureBoxCtrl::OnPaint()
{
	__super::OnPaint();
	InvokeMethodInt( mpTemplate->GetStringProperty( Prop::EventPaint ), (GetFocus() == this? 1 : 0), IsAsyncEvents() );
}
