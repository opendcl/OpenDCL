// VdclTextButton.cpp : implementation file
//

#include "stdafx.h"
#include "VdclTextButton.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "InvokeMethod.h"
#include "ModalVDcl.h"
#include "PropertyIds.h"
#include "ToolTips.h"

extern void CloseModalDlg(CString sFileName, CString sDialogName);


/////////////////////////////////////////////////////////////////////////////
// VdclTextButton

VdclTextButton::VdclTextButton()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
}

VdclTextButton::~VdclTextButton()
{
}


BEGIN_MESSAGE_MAP(VdclTextButton, CButton)
	//{{AFX_MSG_MAP(VdclTextButton)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT(BN_DOUBLECLICKED, OnDoubleclicked)
	ON_WM_KILLFOCUS()
	ON_WM_CAPTURECHANGED()
	ON_WM_NCPAINT()
	ON_WM_HELPINFO()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclTextButton message handlers

BOOL VdclTextButton::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	DWORD dwStyle;
	CRect ArxRect;

	// set the arx control pointer
  m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	// get the caption
	CString Caption = pControl->GetStrProperty(nCaption);
	
	dwStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_CLIPSIBLINGS | BS_MULTILINE;	
	
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	RetVal =  CButton::Create(Caption, dwStyle, ArxRect, pParentWnd, nID );

	VERIFY(CButton::SubclassDlgItem(nID, pParentWnd));
	
	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	int n = m_ArxControl->GetLngProperty(nEventInvoke);
	switch (n)
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	case -1:
	default:
		m_bInvokeWithSendString = false;
		break;
	}
	
	return RetVal;
}

void VdclTextButton::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	CButton::OnDestroy();
}

void VdclTextButton::OnClicked() 
{
	
	if (m_ArxControl->m_bEventsAsAction)
	{
		CString sText = m_ArxControl->GetStrProperty(nEventClicked);

		GetParent()->GetParent()->EnableWindow(TRUE);
		int stat;
		struct resbuf *result = NULL, *list;    

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
		InvokeMethod(m_ArxControl->GetStrProperty(nEventClicked), m_bInvokeWithSendString);	
	}
}

void VdclTextButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	CButton::OnMouseMove(nFlags, point);
}

void VdclTextButton::OnDoubleclicked() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventDblClicked), false);	
}

void VdclTextButton::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	
	ModifyStyle(BS_DEFPUSHBUTTON, 0, SWP_FRAMECHANGED);
	   	
}


void VdclTextButton::OnCaptureChanged(CWnd *pWnd) 
{
	CButton::OnCaptureChanged(pWnd);
}

void VdclTextButton::OnNcPaint() 
{
	// Draw the focus circle on the inside of the knob
	if(GetFocus() != this)
	{
		ModifyStyle(BS_DEFPUSHBUTTON, 0, SWP_FRAMECHANGED);
		Invalidate();
		
	
	}
	else
	{
		ModifyStyle(0, BS_DEFPUSHBUTTON, SWP_FRAMECHANGED);
		Invalidate();
	}
	
	// Do not call CButton::OnNcPaint() for painting messages
}

BOOL VdclTextButton::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CButton::PreTranslateMessage(pMsg);
}

BOOL VdclTextButton::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	/*
	acedHelp(
    char* filename,
    char* topic, int command);
	*/
	return CButton::OnHelpInfo(pHelpInfo);
}

void VdclTextButton::SetDragnDrop(BOOL bRegister)
{
	if (bRegister == TRUE)
	{
		BOOL success = m_DropTarget.Register(this);
		m_DropTarget.m_pThisArxControl = m_ArxControl;
		m_DropTarget.m_pParent = this;
    }
	else
		m_DropTarget.Revoke();
}


void VdclTextButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//SetFocus();
	if (m_ArxControl->GetBoolProperty(nDragnDropAllowBegin) == TRUE && nFlags == 1)
	{
		BeginDragnDrop(m_ArxControl, point, m_bInvokeWithSendString);
	}
	
	CButton::OnLButtonDown(nFlags, point);
}
