// VdclPlotStyleTablesComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "VdclPlotStyleTablesComboBox.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "ArxGridCtrl.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// VdclPlotStyleTablesComboBox

VdclPlotStyleTablesComboBox::VdclPlotStyleTablesComboBox()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclPlotStyleTablesComboBox::~VdclPlotStyleTablesComboBox()
{
}


BEGIN_MESSAGE_MAP(VdclPlotStyleTablesComboBox, CAcUiPlotStyleTablesComboBox)
	//{{AFX_MSG_MAP(VdclPlotStyleTablesComboBox)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)	
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclPlotStyleTablesComboBox message handlers

BOOL VdclPlotStyleTablesComboBox::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
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

	ArxRect.bottom += pControl->GetLongProperty(nDropDownHeight);
	if (ArxRect.Height() < 40)
		ArxRect.bottom = ArxRect.top + 40;

	dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
			  | CBS_SORT | CBS_HASSTRINGS | CBS_AUTOHSCROLL
			  | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_NOINTEGRALHEIGHT;
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	RetVal = CAcUiPlotStyleTablesComboBox::Create( dwStyle, ArxRect, pParentWnd, nID );
	
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

BOOL VdclPlotStyleTablesComboBox::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;

	if (rc.Height() < 100)
		rc.bottom = rc.top + 100;

	// set the arx control pointer
    m_ArxControl = NULL;
		
	dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
			  | CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT;
	
	dwStyle = dwStyle | CBS_SORT;		

	dwStyle = dwStyle | WS_GROUP;

	dwStyle = dwStyle | CBS_DROPDOWNLIST;
	
	RetVal = CAcUiPlotStyleTablesComboBox::Create( dwStyle, rc, pParentWnd, nID );

	VERIFY(CAcUiPlotStyleTablesComboBox::SubclassDlgItem(nID, pParentWnd));
	
	return RetVal;
}
int VdclPlotStyleTablesComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAcUiPlotStyleTablesComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	return 0;
}


void VdclPlotStyleTablesComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CAcUiPlotStyleTablesComboBox::OnSetFocus(pOldWnd);
	
	// combobox is gaining focus, highlight text in edit control
	CAcUiPlotStyleTablesComboBox::SetEditSel(0,-1);	

	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);

}


/////////////////////////////////////////////////////////////////////////////
// VdclPlotStyleTablesComboBox message handlers

void VdclPlotStyleTablesComboBox::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	

	CAcUiPlotStyleTablesComboBox::OnDestroy();
	
	
}

void VdclPlotStyleTablesComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_ArxControl)
		InvokeMethodIntIntInt(
			m_ArxControl->GetStrProperty(nEventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
		
	
	CAcUiPlotStyleTablesComboBox::OnMouseMove(nFlags, point);
}

void VdclPlotStyleTablesComboBox::OnSelchange() 
{
	CString sString;
	int nSel = GetCurSel();
	
	GetLBText(nSel, sString);
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


void VdclPlotStyleTablesComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CAcUiPlotStyleTablesComboBox::OnKillFocus(pNewWnd);
	
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
		
}

BOOL VdclPlotStyleTablesComboBox::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		if (m_ArxControl->GetBoolProperty(nReturnAsTab) == TRUE)
			pMsg->wParam = VK_TAB;
	}
	return CAcUiPlotStyleTablesComboBox::PreTranslateMessage(pMsg);
}

void VdclPlotStyleTablesComboBox::OnDropdown() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventDropDown), m_bInvokeWithSendString);	
}
