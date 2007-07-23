// VdclCheckBox.cpp : implementation file
//

#include "stdafx.h"
#include "VdclCheckBox.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// VdclCheckBox

VdclCheckBox::VdclCheckBox()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
}

VdclCheckBox::~VdclCheckBox()
{
}


BEGIN_MESSAGE_MAP(VdclCheckBox, CClrButton)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// VdclCheckBox message handlers

BOOL VdclCheckBox::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	DWORD dwStyle;
	CRect ArxRect;

	// set the arx control pointer
	m_ArxControl = pControl;
	m_pValue = m_ArxControl->GetPropertyObject(nValue);
	
	// get the rectangle of the new control
	ArxRect.top = pControl->GetPropertyObject(nTop)->GetLongValue();
	ArxRect.left = pControl->GetPropertyObject(nLeft)->GetLongValue();
	ArxRect.bottom = pControl->GetPropertyObject(nHeight)->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->GetPropertyObject(nWidth)->GetLongValue() + ArxRect.left;
	
	// get the caption
	CString Caption = pControl->GetStrProperty(nCaption);
	
		
	dwStyle = WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_CLIPSIBLINGS;	
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	RetVal =  CClrButton::Create(Caption, dwStyle, ArxRect, pParentWnd, nID );

	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	switch (m_ArxControl->GetLongProperty(nEventInvoke))
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

void VdclCheckBox::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	CClrButton::OnDestroy();
}

void VdclCheckBox::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	CClrButton::OnMouseMove(nFlags, point);
}

void VdclCheckBox::OnClicked() 
{
	int nValue = CClrButton::GetCheck();
	m_pValue->SetBooleanValue(nValue);

	if (m_ArxControl->m_bEventsAsAction)
	{
		GetParent()->GetParent()->EnableWindow(TRUE);
		int stat;
		struct resbuf *result = NULL, *list;    
		CString sVal;
		sVal.Format(_T("%d"), nValue);
    struct resbuf *VarVal = acutBuildList(RTSTR, (LPCTSTR)sVal, 0);
    stat = acedPutSym(_T("$value"), VarVal);
    acutRelRb(VarVal);

    CString sText = m_ArxControl->GetStrProperty(nEventClicked);

		list = acutBuildList(RTSTR, sText, 0);
		if (list != NULL) 
		{ 
			stat = acedInvoke(list, &result); 
			acutRelRb(list); 
			if(result != NULL) 
			{
				acutRelRb(result); 
			}
		}
		GetParent()->GetParent()->EnableWindow(FALSE);
		GetParent()->EnableWindow(TRUE);
	}
	else
	{
		// call methods to invoke the event
		InvokeMethodInt(
			m_ArxControl->GetStrProperty(nEventClicked),
			nValue,
			m_bInvokeWithSendString);
	}
}

void VdclCheckBox::OnDoubleclicked() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventDblClicked), m_bInvokeWithSendString);
}

BOOL VdclCheckBox::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
		pMsg->wParam = VK_TAB;		
	return CClrButton::PreTranslateMessage(pMsg);
}
