// VdclComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "VdclComboBox.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "ArxGridCtrl.h"
#include "PropertyIds.h"

const int CmboStyle_Simple		= 1;


/////////////////////////////////////////////////////////////////////////////
// VdclComboBox

VdclComboBox::VdclComboBox()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
	m_bESC = false;
	m_pGridEdit = NULL;
	m_bAutoComplete = true;
}

VdclComboBox::~VdclComboBox()
{
}


BEGIN_MESSAGE_MAP(VdclComboBox, CComboBox)
	//{{AFX_MSG_MAP(VdclComboBox)
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditchange)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditupdate)
	ON_WM_SHOWWINDOW()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclComboBox message handlers


BOOL VdclComboBox::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
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
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
			  | CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT;
	
	if (pControl->GetBoolProperty(nSorted) == TRUE)
		dwStyle = dwStyle | CBS_SORT;		

	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	switch (pControl->GetLngProperty(nComboBoxStyle))
	{
	case 0:
		{
			ArxRect.bottom += pControl->GetLngProperty(nDropDownHeight);
			if (ArxRect.Height() < 40)
				ArxRect.bottom = ArxRect.top + 40;
			dwStyle = dwStyle | CBS_DROPDOWN;
			break;
		}
	case 1:
		{
			dwStyle = dwStyle | CBS_SIMPLE;
			break;
		}
	default:
		{
			ArxRect.bottom += pControl->GetLngProperty(nDropDownHeight);
			//if (ArxRect.Height() < 40)
			//	ArxRect.bottom = ArxRect.top + 40;
			dwStyle = dwStyle | CBS_DROPDOWNLIST;
			break;
		}
	}

	RetVal = CComboBox::Create( dwStyle, ArxRect, pParentWnd, nID );

	LimitText(pControl->GetLngProperty(nLimitText));
	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	SetExtendedUI(TRUE);
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

BOOL VdclComboBox::Create(CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;

	//if (rc.Height() < 100)
	//	rc.bottom = rc.top + 200;

	// set the arx control pointer
    m_ArxControl = NULL;
	dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT;

	dwStyle = dwStyle | WS_GROUP;

	dwStyle = dwStyle | CBS_DROPDOWNLIST;
	
	try
	{
	RetVal = CComboBox::Create( dwStyle, rc, pParentWnd, nID );
	}
	catch(...)
	{
	}
	VERIFY(CComboBox::SubclassDlgItem(nID, pParentWnd));
	
	return RetVal;
}

void VdclComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	CComboBox::OnMouseMove(nFlags, point);

	if (m_ArxControl)
		InvokeMethodIntIntInt(
			m_ArxControl->GetStrProperty(nEventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
		
	

}
void VdclComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CComboBox::OnSetFocus(pOldWnd);
	
	// combobox is gaining focus, highlight text in edit control
	//CComboBox::SetEditSel(0,-1);	
}

void VdclComboBox::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	

	CComboBox::OnDestroy();
	
}

void VdclComboBox::OnEditchange() 
{
	CString sText;
	
	GetWindowText(sText);
	if (m_ArxControl)
	{
		CString sTestText = m_ArxControl->GetStrProperty(nText);
		if (sTestText != sText && m_ArxControl)
		{
			m_ArxControl->SetStringProperty(nText, sText);
			// call methods to invoke the event
			InvokeMethodString(m_ArxControl->GetStrProperty(nEventEditChanged), sText, m_bInvokeWithSendString);
		}
	}
	else
	{
		// Send Notification to parent of ListView ctrl
		CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();
		pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sText);	
		
		// fire the on Grid edit cell event.
		pListCtrl->EndEditControls(pListCtrl);
	}

}


void VdclComboBox::OnKillfocus() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);

}

void VdclComboBox::OnSetfocus() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);

}

void VdclComboBox::OnSelchange() 
{
	CString sString;
	int nSel = GetCurSel();
	int nTextLength = GetLBTextLen(nSel);      
	GetLBText(nSel, sString.GetBuffer(nTextLength));
	sString.ReleaseBuffer();

	if (m_ArxControl)
	{
		if (m_ArxControl->m_bEventsAsAction)
		{
			GetParent()->GetParent()->EnableWindow(TRUE);
			int stat;
			struct resbuf *result = NULL, *list;    
			CString sVal;
			sVal.Format(_T("%d"), nSel);
      struct resbuf *VarVal = acutBuildList(RTSTR, (LPCTSTR)sVal, 0);
      stat = acedPutSym(_T("$value"), VarVal);
      acutRelRb(VarVal);

			CString sText = m_ArxControl->GetStrProperty(nEventSelChanged);

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
			
			InvokeMethodIntString(m_ArxControl->GetStrProperty(nEventSelChanged), nSel, sString, m_bInvokeWithSendString);
			m_ArxControl->SetStringProperty(nText, sString);
		
		}
	}
	
	if (m_ArxControl == NULL)
	{		
		// Send Notification to parent of ListView ctrl
		CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();
		pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sString);	
		
		if (m_pGridEdit != NULL)
		{
			m_pGridEdit->SetWindowText(sString);
		}
		// fire the on Grid edit cell event.
		pListCtrl->EndEditControls(pListCtrl);
	}

	
}

void VdclComboBox::OnEditupdate() 
{
	
	if (m_ArxControl)
	{
		CString sText;
		
		GetWindowText(sText);
		CString sTestText = m_ArxControl->GetStrProperty(nText);
		if (sTestText != sText)
		{
			m_ArxControl->SetStringProperty(nText, sText);
			// call methods to invoke the event
			InvokeMethodString(m_ArxControl->GetStrProperty(nEventEditChanged), sText, m_bInvokeWithSendString);
		}
	}
	// if we are not to auto update the text, get outta here
	if (!m_bAutoComplete) 
		return;

	// Get the text in the edit box
	CString str;
	GetWindowText(str);
	int nLength = str.GetLength();

	// Currently selected range
	DWORD dwCurSel = GetEditSel();
	WORD dStart = LOWORD(dwCurSel);
	WORD dEnd   = HIWORD(dwCurSel);

	// Search for, and select in, and string in the combo box that is prefixed
	// by the text in the edit box
	if (SelectString(-1, str) == CB_ERR)
	{
		SetWindowText(str);		// No text selected, so restore what was there before
		if (dwCurSel != CB_ERR)
		SetEditSel(dStart, dEnd);	//restore cursor postion
	}

	// Set the text selection as the additional text that we have added
	if (dEnd < nLength && dwCurSel != CB_ERR)
		SetEditSel(dStart, dEnd);
	else
		SetEditSel(nLength, -1);

}



void VdclComboBox::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if (m_ArxControl)
	{
		if (m_ArxControl->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
		{		
 			CRect rcThis;
			GetWindowRect(&rcThis);
			GetParent()->ScreenToClient(rcThis);
			
			CRect rcListBox;
			GetClientRect(rcListBox);
			rcThis.bottom = rcThis.bottom -1;

			MoveWindow(rcThis, TRUE);
		}
	}
	
	CComboBox::OnShowWindow(bShow, nStatus);	
}

BOOL VdclComboBox::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	
	if (pMsg->message == WM_KEYDOWN)
	{
		m_bAutoComplete = TRUE;

		int nVirtKey = (int) pMsg->wParam;
		if (nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
			m_bAutoComplete = false;
	}

	
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN && m_ArxControl)
	{
		if (m_ArxControl->GetBoolProperty(nReturnAsTab) == TRUE)
			pMsg->wParam = VK_TAB;		
	}
	if (pMsg->message== WM_KEYDOWN && m_ArxControl == NULL)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			m_bESC = pMsg->wParam == VK_ESCAPE;
			return TRUE;				// DO NOT process further
		}
	}

	return CComboBox::PreTranslateMessage(pMsg);
}



void VdclComboBox::OnDropdown() 
{

	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventDropDown), m_bInvokeWithSendString);	
}
