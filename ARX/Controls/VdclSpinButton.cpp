// VdclSpinButton.cpp : implementation file
//

#include "stdafx.h"
#include "VdclSpinButton.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"
#include "ToolTips.h"


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

BOOL VdclSpinButton::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
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
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | WS_CLIPSIBLINGS | WS_GROUP ;
	
	if (pControl->GetLngProperty(nOrientation) == 0)
		dwStyle = dwStyle | UDS_HORZ;
	if (pControl->GetBoolProperty(nAutoWrap))
		dwStyle = dwStyle | UDS_WRAP;

	
	m_pWrapProp = pControl->GetPropertyObject(nAutoWrap);
	m_pMinProp = pControl->GetPropertyObject(nMinValue);
	m_pMaxProp = pControl->GetPropertyObject(nMaxValue);

	m_Pos = pControl->GetLngProperty(nValue);
	
	RetVal = CSpinButtonCtrl::Create(dwStyle, ArxRect, pParentWnd, nID );

	m_pValueProp = pControl->GetPropertyObject(nValue);

	SetPos(m_Pos);

	InitToolTip();
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
	InvokeMethodIntInt(m_ArxControl->GetStrProperty(nEventChanged), m_Pos, pNMUpDown->iDelta, m_bInvokeWithSendString);
	
	*pResult = 0;
}


void VdclSpinButton::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	
	CSpinButtonCtrl::OnMouseMove(nFlags, point);
}
void VdclSpinButton::SetTooltipText(CString* spText, BOOL bActivate)
{
} // End of SetTooltipText
void VdclSpinButton::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
		m_ToolTip.Create(this);
} // End of InitToolTip


BOOL VdclSpinButton::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
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
