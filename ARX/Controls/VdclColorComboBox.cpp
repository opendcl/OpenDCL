// VdclColorComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "VdclColorComboBox.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "SpreadSheet.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// VdclColorComboBox

VdclColorComboBox::VdclColorComboBox()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclColorComboBox::~VdclColorComboBox()
{
}


BEGIN_MESSAGE_MAP(VdclColorComboBox, CAcUiColorComboBox)
	//{{AFX_MSG_MAP(VdclColorComboBox)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)	
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclColorComboBox message handlers

void VdclColorComboBox::DrawComboItemImage(CDC& dc, CRect& r, int cargo)
{
	DrawItemImage(dc, r, cargo);
}

BOOL VdclColorComboBox::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	DWORD dwStyle;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	pControl->SetLongProperty(nHeight, (pControl->GetLngProperty(nHeight) + pControl->GetLngProperty(nDropDownHeight)));
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	

	if (ArxRect.Height() < 40)
	{
		ArxRect.bottom = ArxRect.top + 60;
	}
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_CLIPSIBLINGS
			  | CBS_HASSTRINGS | WS_TABSTOP | ES_AUTOHSCROLL
			  | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED;
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	RetVal = CAcUiColorComboBox::Create( dwStyle, ArxRect, pParentWnd, nID );
	

	//VERIFY(CAcUiColorComboBox::SubclassDlgItem(nID, pParentWnd));
	
	InitToolTip();
	SetToolTipEx(this, m_ToolTip, pControl);

	SetUseOther1(TRUE);
	SetUseOther2(TRUE);

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
BOOL VdclColorComboBox::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;

	if (rc.Height() < 100)
		rc.bottom = rc.top + 100;

	// set the arx control pointer
    m_ArxControl = NULL;
		
	dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS
			  | CBS_HASSTRINGS | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | WS_GROUP |WS_TABSTOP;
	
	RetVal = CAcUiColorComboBox::Create( dwStyle, rc, pParentWnd, nID );

	VERIFY(CAcUiColorComboBox::SubclassDlgItem(nID, pParentWnd));
	
	return RetVal;
}
int VdclColorComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAcUiColorComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	return 0;
}


void VdclColorComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CAcUiColorComboBox::OnSetFocus(pOldWnd);
	
	// combobox is gaining focus, highlight text in edit control
	CAcUiColorComboBox::SetEditSel(0,-1);	

	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);

}


void VdclColorComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_ArxControl)
		InvokeMethodIntIntInt(
			m_ArxControl->GetStrProperty(nEventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
		
	
	CAcUiColorComboBox::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
// VdclColorComboBox message handlers

void VdclColorComboBox::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
	CAcUiColorComboBox::OnDestroy();
	
	
}


void VdclColorComboBox::OnSelchange() 
{
	CString sString;
	int nSel = GetCurSel();
	int nTextLength = GetLBTextLen(nSel);      
	GetLBText(nSel, sString.GetBuffer(nTextLength));
	sString.ReleaseBuffer();

	if (m_ArxControl)
	{
		InvokeMethodIntString(m_ArxControl->GetStrProperty(nEventSelChanged), nSel, sString, m_bInvokeWithSendString);
		m_ArxControl->SetStringProperty(nText, sString);
	}

	if (m_ArxControl == NULL)
	{		
		// Send Notification to parent of ListView ctrl
		CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent()->GetParent();

		pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, GetCurrentItemColorIndex());
		pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sString);
		// fire the on Grid edit cell event.
		pListCtrl->EndEditControls(pListCtrl);
	}
}



void VdclColorComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CAcUiColorComboBox::OnKillFocus(pNewWnd);
	
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
	
}

COLORREF VdclColorComboBox::GetColorFromIndex(int nColorIndex)
{
	return CAcUiColorComboBox::GetColorFromIndex(nColorIndex);
}
void VdclColorComboBox::SetTooltipText(CString* spText, BOOL bActivate)
{
	
} // End of SetTooltipText
void VdclColorComboBox::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
		m_ToolTip.Create(this);
} // End of InitToolTip

BOOL VdclColorComboBox::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);	

	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		if (m_ArxControl->GetBoolProperty(nReturnAsTab) == TRUE)
			pMsg->wParam = VK_TAB;
		
	}

	return CAcUiColorComboBox::PreTranslateMessage(pMsg);
}
void VdclColorComboBox::OnDropdown() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventDropDown), m_bInvokeWithSendString);	
}
