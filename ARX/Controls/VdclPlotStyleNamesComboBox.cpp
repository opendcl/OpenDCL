// VdclPlotStyleNamesComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "VdclPlotStyleNamesComboBox.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "ArxGridCtrl.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// VdclPlotStyleNamesComboBox

VdclPlotStyleNamesComboBox::VdclPlotStyleNamesComboBox()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclPlotStyleNamesComboBox::~VdclPlotStyleNamesComboBox()
{
}


BEGIN_MESSAGE_MAP(VdclPlotStyleNamesComboBox, CAcUiPlotStyleNamesComboBox)
	//{{AFX_MSG_MAP(VdclPlotStyleNamesComboBox)
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
// VdclPlotStyleNamesComboBox message handlers

BOOL VdclPlotStyleNamesComboBox::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
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

	ArxRect.bottom = ArxRect.bottom  + pControl->GetLngProperty(nDropDownHeight);
	if (ArxRect.Height() < 40)
		ArxRect.bottom = ArxRect.top + 60;

	dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_EX_CLIENTEDGE  | WS_CLIPSIBLINGS
			  | CBS_HASSTRINGS | ES_AUTOHSCROLL
			  | CBS_DROPDOWNLIST;// | CBS_OWNERDRAWFIXED | CBS_SORT;
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	RetVal = CAcUiPlotStyleNamesComboBox::Create( dwStyle, ArxRect, pParentWnd, nID );
	
	VERIFY(CAcUiPlotStyleNamesComboBox::SubclassDlgItem(nID, pParentWnd));

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

BOOL VdclPlotStyleNamesComboBox::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;

	if (rc.Height() < 100)
		rc.bottom = rc.top + 100;

	// set the arx control pointer
    m_ArxControl = NULL;
		
	dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS
			  | CBS_HASSTRINGS | ES_AUTOHSCROLL | WS_CLIPCHILDREN;
	
	dwStyle = dwStyle | CBS_SORT;		

	dwStyle = dwStyle | WS_GROUP;

	dwStyle = dwStyle | CBS_DROPDOWNLIST;
	
	RetVal = CAcUiPlotStyleNamesComboBox::Create( dwStyle, rc, pParentWnd, nID );

	VERIFY(CAcUiPlotStyleNamesComboBox::SubclassDlgItem(nID, pParentWnd));
	
	return RetVal;
}
int VdclPlotStyleNamesComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAcUiPlotStyleNamesComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	return 0;
}


void VdclPlotStyleNamesComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CAcUiPlotStyleNamesComboBox::OnSetFocus(pOldWnd);
	
	// combobox is gaining focus, highlight text in edit control
	CAcUiPlotStyleNamesComboBox::SetEditSel(0,-1);	

	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);

}


/////////////////////////////////////////////////////////////////////////////
// VdclPlotStyleNamesComboBox message handlers

void VdclPlotStyleNamesComboBox::OnDestroy() 
{

	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
	CAcUiPlotStyleNamesComboBox::OnDestroy();
	
	
}

void VdclPlotStyleNamesComboBox::OnSelchange() 
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
		LV_DISPINFO dispinfo;
		dispinfo.hdr.hwndFrom = GetParent()->GetParent()->m_hWnd;
		dispinfo.hdr.idFrom = GetDlgCtrlID();
		dispinfo.hdr.code = LVN_ENDLABELEDIT;

		CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();
		dispinfo.item.mask = LVIF_TEXT;
		dispinfo.item.iItem = pListCtrl->m_nRowSelected;
		dispinfo.item.iSubItem = pListCtrl->m_nColSelected;
		dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)sString);
		dispinfo.item.cchTextMax = sString.GetLength();

		pListCtrl->SendMessage( WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );
		pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sString);
		pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, -1);

		// fire the on Grid edit cell event.
		pListCtrl->EndEditControls(pListCtrl);
	}
}


void VdclPlotStyleNamesComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CAcUiPlotStyleNamesComboBox::OnKillFocus(pNewWnd);
	
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
	
}


void VdclPlotStyleNamesComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_ArxControl)
		InvokeMethodIntIntInt(
			m_ArxControl->GetStrProperty(nEventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
	
	
	CAcUiPlotStyleNamesComboBox::OnMouseMove(nFlags, point);
}

void VdclPlotStyleNamesComboBox::SetTooltipText(CString* spText, BOOL bActivate)
{
} // End of SetTooltipText
void VdclPlotStyleNamesComboBox::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
		m_ToolTip.Create(this);
} // End of InitToolTip

BOOL VdclPlotStyleNamesComboBox::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		if (m_ArxControl->GetBoolProperty(nReturnAsTab) == TRUE)
			pMsg->wParam = VK_TAB;
		
	}

	return CAcUiPlotStyleNamesComboBox::PreTranslateMessage(pMsg);
}
void VdclPlotStyleNamesComboBox::OnDropdown() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventDropDown), m_bInvokeWithSendString);	
}
