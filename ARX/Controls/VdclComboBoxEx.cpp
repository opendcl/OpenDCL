// VdclComboBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "VdclComboBoxEx.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "SpreadSheet.h"
#include "PropertyIds.h"

const int CmboStyle_Simple		= 1;


/////////////////////////////////////////////////////////////////////////////
// VdclComboBoxEx

VdclComboBoxEx::VdclComboBoxEx()
{
	m_bESC = false;
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclComboBoxEx::~VdclComboBoxEx()
{
}


BEGIN_MESSAGE_MAP(VdclComboBoxEx, CComboBoxEx)
	//{{AFX_MSG_MAP(VdclComboBoxEx)
	ON_WM_CREATE()
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
// VdclComboBoxEx message handlers


BOOL VdclComboBoxEx::Create2(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	DWORD dwStyle;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();

	/*if (pControl->GetLngProperty(nComboBoxStyle) != 1)
	{
		pControl->SetLngProperty(nHeight, (pControl->GetLngProperty(nHeight) + pControl->GetLngProperty(nDropDownHeight)));
	}*/

	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	if (ArxRect.Height() < 40)
	{
		ArxRect.bottom = ArxRect.top + 60;
	}
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS
			  | ES_AUTOHSCROLL | WS_CLIPCHILDREN;
	
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
			ArxRect.bottom = ArxRect.bottom  + pControl->GetLngProperty(nDropDownHeight);
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
			ArxRect.bottom = ArxRect.bottom  + pControl->GetLngProperty(nDropDownHeight);
			dwStyle = dwStyle | CBS_DROPDOWNLIST;
			break;
		}
	}

	RetVal = CComboBoxEx::Create( dwStyle, ArxRect, pParentWnd, nID );


	VERIFY(CComboBoxEx::SubclassDlgItem(nID, pParentWnd));
	LimitText(pControl->GetLngProperty(nLimitText));
	InitToolTip();
	SetToolTipEx(this, m_ToolTip, pControl);

	//SetExtendedUI(TRUE);
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


int VdclComboBoxEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBoxEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	
	return 0;
}


void VdclComboBoxEx::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (m_ArxControl)
	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(nEventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	
	CComboBoxEx::OnMouseMove(nFlags, point);
}
void VdclComboBoxEx::OnSetFocus(CWnd* pOldWnd) 
{
	CComboBoxEx::OnSetFocus(pOldWnd);
	
	// ComboBoxEx is gaining focus, highlight text in edit control
	//CComboBoxEx::SetEditSel(0,-1);	
}

void VdclComboBoxEx::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
	SetImageList(NULL);

	CComboBoxEx::OnDestroy();
	
}

void VdclComboBoxEx::OnEditchange() 
{
	CString sText;
	
	GetWindowText(sText);

	if (m_ArxControl)
	{
		CString sTestText = m_ArxControl->GetStrProperty(nText);
		if (sTestText != sText)
		{
			m_ArxControl->SetStrProperty(nText, sText);
			// call methods to invoke the event
			InvokeMethodString(m_ArxControl->GetStrProperty(nEventEditChanged), sText, m_bInvokeWithSendString);
		}
	}
}


void VdclComboBoxEx::OnKillfocus() 
{
	// call methods to invoke the event
	if (m_ArxControl)
		InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
}

void VdclComboBoxEx::OnSetfocus() 
{
	// call methods to invoke the event
	if (m_ArxControl)
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);

}

void VdclComboBoxEx::OnSelchange() 
{
#ifdef _UNICODE	
  COMBOBOXEXITEMW cbi;
#else
	COMBOBOXEXITEMA cbi;
#endif
	CString			sString;
	int				nImage;
	int				nSel = GetCurSel();

	cbi.mask = CBEIF_IMAGE;

	cbi.iItem = nSel;
	
	GetItem(&cbi);

	int n = GetLBTextLen(nSel);      
	GetLBText(nSel, sString.GetBuffer(n));
	sString.ReleaseBuffer();
	LPTSTR str;
	str = cbi.pszText;// = (LPTSTR)(LPCTSTR)str;
	/*
	//cbi.cchTextMax = str.GetLength();
	sString = str;
	sString = sString.Left(cbi.cchTextMax);
	*/
	//sString = cbi.pszText;
	nImage = cbi.iImage;

	if (m_ArxControl)
	{
		InvokeMethodIntString(m_ArxControl->GetStrProperty(nEventSelChanged), nSel, sString, m_bInvokeWithSendString);
		m_ArxControl->SetStrProperty(nText, sString);
	}
	
	if (m_ArxControl == NULL && !m_bESC)
	{
		CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent()->GetParent();
		
		pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sString);		
		pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, nImage);	
		// fire the on Grid edit cell event.
		pListCtrl->EndEditControls(pListCtrl);
	}
}

void VdclComboBoxEx::OnEditupdate() 
{
	CString sText;
	
	GetWindowText(sText);
	if (m_ArxControl)
	{
		CString sTestText = m_ArxControl->GetStrProperty(nText);
		if (sTestText != sText)
		{
			m_ArxControl->SetStrProperty(nText, sText);
			// call methods to invoke the event
			InvokeMethodString(m_ArxControl->GetStrProperty(nEventEditChanged), sText, m_bInvokeWithSendString);
		}
	}
	
}



void VdclComboBoxEx::OnShowWindow(BOOL bShow, UINT nStatus) 
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
	
	CComboBoxEx::OnShowWindow(bShow, nStatus);	
}

void VdclComboBoxEx::SetTooltipText(CString* spText, BOOL bActivate)
{

} // End of SetTooltipText

void VdclComboBoxEx::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip

BOOL VdclComboBoxEx::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN && m_ArxControl)
	{
		if (m_ArxControl)
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


	return CComboBoxEx::PreTranslateMessage(pMsg);
}



void VdclComboBoxEx::OnDropdown() 
{
	// call methods to invoke the event
	if (m_ArxControl)
		InvokeMethod(m_ArxControl->GetStrProperty(nEventDropDown), m_bInvokeWithSendString);	
}
