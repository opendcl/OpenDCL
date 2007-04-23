// VdclLineWeightComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "VdclLineWeightComboBox.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "ArxGridCtrl.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// VdclLineWeightComboBox

VdclLineWeightComboBox::VdclLineWeightComboBox()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclLineWeightComboBox::~VdclLineWeightComboBox()
{
}


BEGIN_MESSAGE_MAP(VdclLineWeightComboBox, CAcUiLineWeightComboBox)
	//{{AFX_MSG_MAP(VdclLineWeightComboBox)
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
// VdclLineWeightComboBox message handlers

BOOL VdclLineWeightComboBox::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
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

	ArxRect.bottom += pControl->GetLngProperty(nDropDownHeight);
	if (ArxRect.Height() < 40)
		ArxRect.bottom = ArxRect.top + 40;
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
			  | CBS_SORT | CBS_HASSTRINGS | CBS_AUTOHSCROLL
			  | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_NOINTEGRALHEIGHT;
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	RetVal = CAcUiLineWeightComboBox::Create( dwStyle, ArxRect, pParentWnd, nID );

	CAcUiLineWeightComboBox::SetUseDefault(FALSE);
	
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

BOOL VdclLineWeightComboBox::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;

	if (rc.Height() < 100)
		rc.bottom = rc.top + 100;

	// set the arx control pointer
    m_ArxControl = NULL;
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS
			  | CBS_HASSTRINGS| CBS_AUTOHSCROLL 
			  | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_NOINTEGRALHEIGHT
				| WS_GROUP | WS_TABSTOP;
	
	RetVal = CAcUiLineWeightComboBox::Create( dwStyle, rc, pParentWnd, nID );

	VERIFY(CAcUiLineWeightComboBox::SubclassDlgItem(nID, pParentWnd));
	
	return RetVal;
}
int VdclLineWeightComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAcUiLineWeightComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//CFont *pFont = GetFontCollection()->GetFont(m_ArxControl, this);
	//SetFont(pFont);
		
	return 0;
}


void VdclLineWeightComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_ArxControl)
		InvokeMethodIntIntInt(
			m_ArxControl->GetStrProperty(nEventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
	
	CAcUiLineWeightComboBox::OnMouseMove(nFlags, point);
}
void VdclLineWeightComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CAcUiLineWeightComboBox::OnSetFocus(pOldWnd);
	
	// combobox is gaining focus, highlight text in edit control
	CAcUiLineWeightComboBox::SetEditSel(0,-1);	

	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);

}


/////////////////////////////////////////////////////////////////////////////
// VdclLineWeightComboBox message handlers

void VdclLineWeightComboBox::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	

	CAcUiLineWeightComboBox::OnDestroy();
	
	
}


void VdclLineWeightComboBox::OnSelchange() 
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
		CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();
		pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sString);		
		pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, nSel);
		// fire the on Grid edit cell event.
		pListCtrl->EndEditControls(pListCtrl);
	}
}

void VdclLineWeightComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CAcUiLineWeightComboBox::OnKillFocus(pNewWnd);
	
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
	
	
}

void VdclLineWeightComboBox::SetTooltipText(CString* spText, BOOL bActivate)
{
} // End of SetTooltipText
void VdclLineWeightComboBox::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
		m_ToolTip.Create(this);
} // End of InitToolTip


BOOL VdclLineWeightComboBox::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		if (m_ArxControl->GetBoolProperty(nReturnAsTab) == TRUE)
			pMsg->wParam = VK_TAB;
		
	}

	return CAcUiLineWeightComboBox::PreTranslateMessage(pMsg);
}
void VdclLineWeightComboBox::OnDropdown() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventDropDown), m_bInvokeWithSendString);	
}
