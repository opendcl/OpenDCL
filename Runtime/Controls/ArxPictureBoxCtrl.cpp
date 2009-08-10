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
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseDown, 1, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventLMouse, 1, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	__super::OnLButtonDown( nFlags, point );
}

void CArxPictureBoxCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	if( HandleCtrlEvent( mpTemplate, Prop::EventKillFocus, IsAsyncEvents() ) )
		return;
}

void CArxPictureBoxCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus(pOldWnd);
	if( HandleCtrlEvent( mpTemplate, Prop::EventSetFocus, IsAsyncEvents() ) )
		return;
}

void CArxPictureBoxCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseUp, 1, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventLMouse, 1, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	__super::OnLButtonUp(nFlags, point);
}

void CArxPictureBoxCtrl::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseDblClick, 4, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventMMouse, 4, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	__super::OnMButtonDblClk(nFlags, point);
}

void CArxPictureBoxCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseDown, 4, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventMMouse, 4, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	__super::OnMButtonDown(nFlags, point);
}

void CArxPictureBoxCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseUp, 4, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventMMouse, 4, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
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
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseMove, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	__super::OnMouseMove(nFlags, point);
}

LRESULT CArxPictureBoxCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	mbTrackingMouse = false;        
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseMovedOff, IsAsyncEvents() ) )
		return FALSE;
	return FALSE;
}

BOOL CArxPictureBoxCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseDblClick, nFlags, zDelta, pt.x, pt.y, IsAsyncEvents() ) )
		return FALSE;
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CArxPictureBoxCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseDblClick, 2, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventRightDblClick, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventRMouse, 2, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	__super::OnRButtonDblClk(nFlags, point);
}

void CArxPictureBoxCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseDown, 2, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventRMouse, 2, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	__super::OnRButtonDown(nFlags, point);
}

void CArxPictureBoxCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseUp, 2, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventRightClick, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventRMouse, 2, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	__super::OnRButtonUp(nFlags, point);
}

void CArxPictureBoxCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TCHAR sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyDown), sChar, nRepCnt, nFlags, IsAsyncEvents());
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxPictureBoxCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TCHAR sChar = nChar;
	InvokeMethodStringIntInt(mpTemplate->GetStringProperty(Prop::EventKeyUp), sChar, nRepCnt, nFlags, IsAsyncEvents());
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxPictureBoxCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( HandleCtrlEvent( mpTemplate, Prop::EventMouseDblClick, 1, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventDblClicked, IsAsyncEvents() ) )
		return;
	if( HandleCtrlEvent( mpTemplate, Prop::EventLMouse, 1, nFlags, point.x, point.y, IsAsyncEvents() ) )
		return;
	__super::OnLButtonDblClk(nFlags, point);
}

void CArxPictureBoxCtrl::OnClicked() 
{
	if( HandleCtrlEvent( mpTemplate, Prop::EventClicked, IsAsyncEvents() ) )
		return;
}

void CArxPictureBoxCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == 13 || nChar == 10 )
	{
		if( HandleCtrlEvent( mpTemplate, Prop::EventClicked, IsAsyncEvents() ) )
			return;
	}
}

void CArxPictureBoxCtrl::OnPaint()
{
	__super::OnPaint();
	InvokeMethodBoolean( mpTemplate->GetStringProperty( Prop::EventPaint ), (GetFocus() == this), IsAsyncEvents() );
}
