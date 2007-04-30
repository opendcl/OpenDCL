// VdclSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VdclSliderCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ToolTips.h"


/////////////////////////////////////////////////////////////////////////////
// VdclSliderCtrl

VdclSliderCtrl::VdclSliderCtrl()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclSliderCtrl::~VdclSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(VdclSliderCtrl, CSliderCtrl)
	//{{AFX_MSG_MAP(VdclSliderCtrl)
	ON_NOTIFY_REFLECT(NM_OUTOFMEMORY, OnOutofmemory)
	ON_NOTIFY_REFLECT(NM_RELEASEDCAPTURE, OnReleasedcapture)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclSliderCtrl message handlers

BOOL VdclSliderCtrl::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
		
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_GROUP;//| TBS_BOTTOM ;
	
	if (pControl->GetBoolProperty(nShowTicks) == TRUE)
		dwStyle = dwStyle | TBS_AUTOTICKS;
	
	if (pControl->GetLngProperty(nOrientation) == 1)
		dwStyle = dwStyle | TBS_VERT;
	else
		dwStyle = dwStyle | TBS_HORZ;

	RetVal = CSliderCtrl::Create(dwStyle, ArxRect, pParentWnd, nID);
	
	SetRange( pControl->GetLngProperty(nMinValue), pControl->GetLngProperty(nMaxValue), TRUE );
	SetLineSize( pControl->GetLngProperty(nSmallChange) );
	SetPageSize( pControl->GetLngProperty(nLargeChange) );
		
	m_pValueProp = pControl->GetPropertyObject(nValue);
	SetPos(m_pValueProp->GetLongValue());

	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);
	
	switch (m_ArxControl->GetLngProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}	
	
	return RetVal;
}

void VdclSliderCtrl::OnOutofmemory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventOutOfMemory), m_bInvokeWithSendString);
	*pResult = 0;
}

void VdclSliderCtrl::OnReleasedcapture(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_pValueProp->SetLongValue(CSliderCtrl::GetPos());
	// call methods to invoke the event
	InvokeMethodInt(m_ArxControl->GetStrProperty(nEventReleasedCapture), 
		CSliderCtrl::GetPos(),
		m_bInvokeWithSendString);
	*pResult = 0;
}

void VdclSliderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);

	// if the mouse if down, send the scroll event to AutoLISP
	if (nFlags != 0)
	{
		m_pValueProp->SetLongValue(CSliderCtrl::GetPos());
		// call methods to invoke the event
		InvokeMethodInt(m_ArxControl->GetStrProperty(nEventScroll), 
			CSliderCtrl::GetPos(),
			m_bInvokeWithSendString);
	}
	CSliderCtrl::OnMouseMove(nFlags, point);
}

void VdclSliderCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	m_pValueProp->SetLongValue(CSliderCtrl::GetPos());
	// call methods to invoke the event
	InvokeMethodInt(m_ArxControl->GetStrProperty(nEventScroll), 
		CSliderCtrl::GetPos(),
		m_bInvokeWithSendString);
	CSliderCtrl::OnLButtonDblClk(nFlags, point);
}

void VdclSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_pValueProp->SetLongValue(CSliderCtrl::GetPos());
	// call methods to invoke the event
	InvokeMethodInt(m_ArxControl->GetStrProperty(nEventScroll), 
		CSliderCtrl::GetPos(),
		m_bInvokeWithSendString);
	CSliderCtrl::OnLButtonDown(nFlags, point);
}

void VdclSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_pValueProp->SetLongValue(CSliderCtrl::GetPos());
	// call methods to invoke the event
	InvokeMethodInt(m_ArxControl->GetStrProperty(nEventScroll), 
		CSliderCtrl::GetPos(),
		m_bInvokeWithSendString);
	CSliderCtrl::OnLButtonUp(nFlags, point);
}

void VdclSliderCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_pValueProp->SetLongValue(CSliderCtrl::GetPos());
	// call methods to invoke the event
	InvokeMethodInt(m_ArxControl->GetStrProperty(nEventScroll), 
		CSliderCtrl::GetPos(),
		m_bInvokeWithSendString);
	CSliderCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void VdclSliderCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_pValueProp->SetLongValue(CSliderCtrl::GetPos());
	// call methods to invoke the event
	InvokeMethodInt(m_ArxControl->GetStrProperty(nEventScroll), 
		CSliderCtrl::GetPos(),
		m_bInvokeWithSendString);
	CSliderCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

BOOL VdclSliderCtrl::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CSliderCtrl::PreTranslateMessage(pMsg);
}

void VdclSliderCtrl::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	CSliderCtrl::OnDestroy();
}
