// VdclRadioButton.cpp : implementation file
//

#include "stdafx.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "VdclRadioButton.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"



/////////////////////////////////////////////////////////////////////////////
// VdclRadioButton

VdclRadioButton::VdclRadioButton()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclRadioButton::~VdclRadioButton()
{
}


BEGIN_MESSAGE_MAP(VdclRadioButton, CClrButton)
	//{{AFX_MSG_MAP(VdclRadioButton)
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclRadioButton message handlers

BOOL VdclRadioButton::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	DWORD dwStyle;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	m_pValue = m_ArxControl->GetPropertyObject(nValue);	

	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	// get the caption
	CString Caption = pControl->GetStrProperty(nCaption);
	
	dwStyle = WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON  | WS_CLIPSIBLINGS;	
	if (pControl->GetBoolProperty(nIsTabStop) == FALSE)		
		dwStyle = dwStyle | WS_GROUP;
	else
		dwStyle = dwStyle | WS_TABSTOP;
	
	RetVal =  CClrButton::Create(Caption, dwStyle, ArxRect, pParentWnd, nID );
	
	VERIFY(CClrButton::SubclassDlgItem(nID, pParentWnd));

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

void VdclRadioButton::OnDestroy() 
{
	int nValue = CClrButton::GetCheck();
	
	m_pValue->SetBooleanValue(nValue != 0);

	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	

	CClrButton::OnDestroy();
	
	
}

void VdclRadioButton::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	
	CClrButton::OnMouseMove(nFlags, point);
}


void VdclRadioButton::OnClicked() 
{
	int nValue = CClrButton::GetCheck();
	
	if (m_ArxControl->m_bEventsAsAction)
	{
		CString sText = m_ArxControl->GetStrProperty(nEventClicked);

		GetParent()->GetParent()->EnableWindow(TRUE);
		int stat;
		struct resbuf *result = NULL, *list;    
		CString sVal;
		sVal.Format(_T("%d"),nValue);
		struct resbuf *VarVal = acutBuildList(RTSTR, (LPCTSTR)sVal, 0);
		stat = acedPutSym(_T("$value"), VarVal);
    acutRelRb(VarVal);
	
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
		if (m_pValue->GetBooleanValue() != (nValue != 0))
		{
			m_pValue->SetBooleanValue(nValue != 0);
			// call methods to invoke the event
			InvokeMethodInt(
				m_ArxControl->GetStrProperty(nEventClicked),
				nValue,
				m_bInvokeWithSendString);
		}
	}
}

void VdclRadioButton::OnDoubleclicked() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventDblClicked), m_bInvokeWithSendString);
	
}


void VdclRadioButton::OnSetFocus(CWnd* pOldWnd) 
{
	CClrButton::OnSetFocus(pOldWnd);
	
	int nValue = CClrButton::GetCheck();
	
	m_pValue->SetBooleanValue(nValue != 0);
	
}

void VdclRadioButton::OnKillFocus(CWnd* pNewWnd) 
{
	CClrButton::OnKillFocus(pNewWnd);
	
	int nValue = CClrButton::GetCheck();
	
	m_pValue->SetBooleanValue(nValue != 0);
	
}

void VdclRadioButton::SetTooltipText(CString* spText, BOOL bActivate)
{
} // End of SetTooltipText
void VdclRadioButton::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip


BOOL VdclRadioButton::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		pMsg->wParam = VK_TAB;		
	}
	
	return CClrButton::PreTranslateMessage(pMsg);
}

