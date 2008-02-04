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
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxPictureBoxCtrl message handlers

void CArxPictureBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( nFlags == MK_LBUTTON && mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) == TRUE )
		BeginDragnDrop(mpTemplate, point, IsAsyncEvents());

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseDown),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnLMouseEvent),
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
		mpTemplate->GetStringProperty(Prop::OnLMouseEvent),
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
		mpTemplate->GetStringProperty(Prop::OnMMouseEvent),
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
		mpTemplate->GetStringProperty(Prop::OnMMouseEvent),
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
		mpTemplate->GetStringProperty(Prop::OnMMouseEvent),
		1,
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	__super::OnMButtonUp(nFlags, point);
}

void CArxPictureBoxCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rcThis;
	GetWindowRect(&rcThis);
	if (point.x < 0 || point.y < 0 || point.x > rcThis.Width() || point.y > rcThis.Height())
	{
		InvokeMethod(mpTemplate->GetStringProperty(Prop::EventMouseMovedOff), IsAsyncEvents());
		return;
	}

	if (!m_bMouseTracking)       
		InvokeMethod(
			mpTemplate->GetStringProperty(Prop::EventMouseEntered),
			IsAsyncEvents());

	InvokeMethodIntIntInt(
		mpTemplate->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());

	__super::OnMouseMove(nFlags, point);
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
		mpTemplate->GetStringProperty(Prop::EventRDblClick),
		IsAsyncEvents());

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnRMouseEvent),
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
		mpTemplate->GetStringProperty(Prop::OnRMouseEvent),
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
		mpTemplate->GetStringProperty(Prop::EventRClick),
		IsAsyncEvents());	

	InvokeMethodIntIntIntInt(
		mpTemplate->GetStringProperty(Prop::OnRMouseEvent),
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

LRESULT CArxPictureBoxCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam) 
{
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventMouseMovedOff), IsAsyncEvents());
	return __super::OnMouseLeave(wParam, lParam);
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
		mpTemplate->GetStringProperty(Prop::OnLMouseEvent),
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
