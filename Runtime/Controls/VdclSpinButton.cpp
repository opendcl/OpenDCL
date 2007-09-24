// VdclSpinButton.cpp : implementation file
//

#include "stdafx.h"
#include "VdclSpinButton.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ToolTips.h"

//needed until this control is derived from CDialogObject
#define IsAsyncEvents() (m_ArxControl->GetLongProperty( Prop::EventInvoke ) == 1)


/////////////////////////////////////////////////////////////////////////////
// VdclSpinButton

VdclSpinButton::VdclSpinButton()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclSpinButton::~VdclSpinButton()
{
}


BEGIN_MESSAGE_MAP(VdclSpinButton, CSpinButtonCtrl)
	//{{AFX_MSG_MAP(VdclSpinButton)
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltapos)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclSpinButton message handlers

BOOL VdclSpinButton::Create(TDclControlPtr pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->GetPropertyObject(Prop::Top)->GetLongValue();
	ArxRect.left = pControl->GetPropertyObject(Prop::Left)->GetLongValue();
	ArxRect.bottom = pControl->GetPropertyObject(Prop::Height)->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->GetPropertyObject(Prop::Width)->GetLongValue() + ArxRect.left;
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | WS_CLIPSIBLINGS | WS_GROUP ;
	
	if (pControl->GetLongProperty(Prop::Orientation) == 0)
		dwStyle = dwStyle | UDS_HORZ;
	if (pControl->GetBooleanProperty(Prop::AutoWrap))
		dwStyle = dwStyle | UDS_WRAP;

	
	m_pWrapProp = pControl->GetPropertyObject(Prop::AutoWrap);
	m_pMinProp = pControl->GetPropertyObject(Prop::MinValue);
	m_pMaxProp = pControl->GetPropertyObject(Prop::MaxValue);

	m_Pos = pControl->GetLongProperty(Prop::Value);
	
	RetVal = CSpinButtonCtrl::Create(dwStyle, ArxRect, pParentWnd, nID );

	m_pValueProp = pControl->GetPropertyObject(Prop::Value);

	SetPos(m_Pos);

	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);
	
	return RetVal;
}

void VdclSpinButton::OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	SetFocus();
	m_Pos = m_Pos - pNMUpDown->iDelta;
	if (m_pWrapProp->GetBooleanValue() == TRUE)
	{	
		if (m_Pos < m_pMinProp->GetLongValue())
			m_Pos = m_pMaxProp->GetLongValue();
		
		if (m_Pos > m_pMaxProp->GetLongValue())
			m_Pos = m_pMinProp->GetLongValue();				
	}
	else
	{	
		if (m_Pos < m_pMinProp->GetLongValue())
			m_Pos = m_pMinProp->GetLongValue();
		
		if (m_Pos > m_pMaxProp->GetLongValue())
			m_Pos = m_pMaxProp->GetLongValue();				
	}

	m_pValueProp->SetLongValue(m_Pos);
	SetPos(m_Pos);
	// call methods to invoke the event
	InvokeMethodIntInt(m_ArxControl->GetStringProperty(Prop::EventChanged), m_Pos, pNMUpDown->iDelta, IsAsyncEvents());
	
	*pResult = 0;
}


void VdclSpinButton::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStringProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		IsAsyncEvents());
	
	
	CSpinButtonCtrl::OnMouseMove(nFlags, point);
}

BOOL VdclSpinButton::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);	

	return CSpinButtonCtrl::PreTranslateMessage(pMsg);
}

void VdclSpinButton::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
	CSpinButtonCtrl::OnDestroy();
	
		
}

void VdclSpinButton::OnSetFocus(CWnd* pOldWnd) 
{
	CSpinButtonCtrl::OnSetFocus(pOldWnd);
	
	
}
