// VdclArrowHeadComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "VdclArrowHeadComboBox.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "ArxGridCtrl.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// VdclArrowHeadComboBox

VdclArrowHeadComboBox::VdclArrowHeadComboBox()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclArrowHeadComboBox::~VdclArrowHeadComboBox()
{
}


BEGIN_MESSAGE_MAP(VdclArrowHeadComboBox, CAcUiArrowHeadComboBox)
	//{{AFX_MSG_MAP(VdclArrowHeadComboBox)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_WM_MEASUREITEM_REFLECT()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclArrowHeadComboBox message handlers

BOOL VdclArrowHeadComboBox::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
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
	

	dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL|WS_CLIPSIBLINGS
			  | CBS_SORT | CBS_HASSTRINGS| ES_AUTOHSCROLL
			  | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |WS_GROUP |WS_TABSTOP;
	
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	RetVal = CAcUiArrowHeadComboBox::Create( dwStyle, ArxRect, pParentWnd, nID );
	
	VERIFY(CAcUiArrowHeadComboBox::SubclassDlgItem(nID, pParentWnd));

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

BOOL VdclArrowHeadComboBox::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;

	if (rc.Height() < 100)
		rc.bottom = rc.top + 100;

	// set the arx control pointer
    m_ArxControl = NULL;
		
	dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS
			  | CBS_HASSTRINGS| ES_AUTOHSCROLL // | CBS_SORT 
			  | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |WS_GROUP |WS_TABSTOP;
	
	RetVal = CAcUiArrowHeadComboBox::Create( dwStyle, rc, pParentWnd, nID );

	VERIFY(CAcUiArrowHeadComboBox::SubclassDlgItem(nID, pParentWnd));
	
	return RetVal;
}

int VdclArrowHeadComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAcUiArrowHeadComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	
	return 0;
}


void VdclArrowHeadComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_ArxControl)
		InvokeMethodIntIntInt(
			m_ArxControl->GetStrProperty(nEventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
		
	
	CAcUiArrowHeadComboBox::OnMouseMove(nFlags, point);
}

void VdclArrowHeadComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CAcUiArrowHeadComboBox::OnSetFocus(pOldWnd);
	
	// combobox is gaining focus, highlight text in edit control
	CAcUiArrowHeadComboBox::SetEditSel(0,-1);	

	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);

}


/////////////////////////////////////////////////////////////////////////////
// VdclArrowHeadComboBox message handlers

void VdclArrowHeadComboBox::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	

	CAcUiArrowHeadComboBox::OnDestroy();
	
	
}




void VdclArrowHeadComboBox::OnSelchange() 
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



void VdclArrowHeadComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CAcUiArrowHeadComboBox::OnKillFocus(pNewWnd);
	
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);

}

void VdclArrowHeadComboBox::SetTooltipText(CString* spText, BOOL bActivate)
{
} // End of SetTooltipText
void VdclArrowHeadComboBox::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
		m_ToolTip.Create(this);
} // End of InitToolTip

BOOL VdclArrowHeadComboBox::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	return CAcUiArrowHeadComboBox::PreTranslateMessage(pMsg);
}
void VdclArrowHeadComboBox::OnDropdown() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventDropDown), m_bInvokeWithSendString);	
}

void VdclArrowHeadComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    lpMeasureItemStruct->itemHeight = 16;
}
