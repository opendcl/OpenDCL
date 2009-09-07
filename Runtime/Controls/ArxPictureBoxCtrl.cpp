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
	if( GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 1, nFlags, point.x, point.y ) ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventLMouse, args_NNNN( 1, nFlags, point.x, point.y ) ) )
		return;
	__super::OnLButtonDown( nFlags, point );
}

void CArxPictureBoxCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	if( GetArxServices()->HandleEvent( Prop::EventKillFocus ) )
		return;
}

void CArxPictureBoxCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus(pOldWnd);
	if( GetArxServices()->HandleEvent( Prop::EventSetFocus ) )
		return;
}

void CArxPictureBoxCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 1, nFlags, point.x, point.y ) ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventLMouse, args_NNNN( 1, nFlags, point.x, point.y ) ) )
		return;
	__super::OnLButtonUp(nFlags, point);
}

void CArxPictureBoxCtrl::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	if( GetArxServices()->HandleEvent( Prop::EventMouseDblClick, args_NNNN( 4, nFlags, point.x, point.y ) ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventMMouse, args_NNNN( 4, nFlags, point.x, point.y ) ) )
		return;
	__super::OnMButtonDblClk(nFlags, point);
}

void CArxPictureBoxCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	if( GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 4, nFlags, point.x, point.y ) ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventMMouse, args_NNNN( 4, nFlags, point.x, point.y ) ) )
		return;
	__super::OnMButtonDown(nFlags, point);
}

void CArxPictureBoxCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	if( GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 4, nFlags, point.x, point.y ) ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventMMouse, args_NNNN( 4, nFlags, point.x, point.y ) ) )
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
		if( GetArxServices()->HandleEvent( Prop::EventMouseEntered ) )
			return;
	}
	if( GetArxServices()->HandleEvent( Prop::EventMouseMove, args_NNN( nFlags, point.x, point.y ) ) )
		return;
	__super::OnMouseMove(nFlags, point);
}

LRESULT CArxPictureBoxCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	mbTrackingMouse = false;        
	if( GetArxServices()->HandleEvent( Prop::EventMouseMovedOff ) )
		return FALSE;
	return FALSE;
}

BOOL CArxPictureBoxCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if( GetArxServices()->HandleEvent( Prop::EventMouseDblClick, args_NNNN( nFlags, zDelta, pt.x, pt.y ) ) )
		return FALSE;
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CArxPictureBoxCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	if( GetArxServices()->HandleEvent( Prop::EventMouseDblClick, args_NNNN( 2, nFlags, point.x, point.y ) ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventRightDblClick ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventRMouse, args_NNNN( 2, nFlags, point.x, point.y ) ) )
		return;
	__super::OnRButtonDblClk(nFlags, point);
}

void CArxPictureBoxCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( GetArxServices()->HandleEvent( Prop::EventMouseDown, args_NNNN( 2, nFlags, point.x, point.y ) ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventRMouse, args_NNNN( 2, nFlags, point.x, point.y ) ) )
		return;
	__super::OnRButtonDown(nFlags, point);
}

void CArxPictureBoxCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if( GetArxServices()->HandleEvent( Prop::EventMouseUp, args_NNNN( 2, nFlags, point.x, point.y ) ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventRightClick ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventRMouse, args_NNNN( 2, nFlags, point.x, point.y ) ) )
		return;
	__super::OnRButtonUp(nFlags, point);
}

void CArxPictureBoxCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	GetArxServices()->HandleEvent( Prop::EventKeyDown, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CArxPictureBoxCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	GetArxServices()->HandleEvent( Prop::EventKeyUp, args_CNN( (TCHAR)nChar, nRepCnt, nFlags ) );
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CArxPictureBoxCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( GetArxServices()->HandleEvent( Prop::EventMouseDblClick, args_NNNN( 1, nFlags, point.x, point.y ) ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventDblClicked ) )
		return;
	if( GetArxServices()->HandleEvent( Prop::EventLMouse, args_NNNN( 1, nFlags, point.x, point.y ) ) )
		return;
	__super::OnLButtonDblClk(nFlags, point);
}

void CArxPictureBoxCtrl::OnClicked() 
{
	if( GetArxServices()->HandleEvent( Prop::EventClicked ) )
		return;
}

void CArxPictureBoxCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == 13 || nChar == 10 )
	{
		if( GetArxServices()->HandleEvent( Prop::EventClicked, IsAsyncEvents() ) )
			return;
	}
}

void CArxPictureBoxCtrl::OnPaint()
{
	__super::OnPaint();
	if( GetArxServices()->HandleEvent( Prop::EventPaint, args_B( (GetFocus() == this) ) ) )
		return;
}
