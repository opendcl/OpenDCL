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
	//ON_WM_SHOWWINDOW()
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
	ArxRect.top = pControl->GetPropertyObject(Prop::Top)->GetLongValue();
	ArxRect.left = pControl->GetPropertyObject(Prop::Left)->GetLongValue();

	ArxRect.bottom = pControl->GetPropertyObject(Prop::Height)->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->GetPropertyObject(Prop::Width)->GetLongValue() + ArxRect.left;
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
			  | CBS_HASSTRINGS | CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT;
	
	if (pControl->GetBooleanProperty(Prop::Sorted) == TRUE)
		dwStyle = dwStyle | CBS_SORT;		

	if (pControl->GetBooleanProperty(Prop::IsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	switch (pControl->GetLongProperty(Prop::ComboBoxStyle))
	{
	case 0:
		{
			ArxRect.bottom += pControl->GetLongProperty(Prop::DropDownHeight);
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
			ArxRect.bottom += pControl->GetLongProperty(Prop::DropDownHeight);
			//if (ArxRect.Height() < 40)
			//	ArxRect.bottom = ArxRect.top + 40;
			dwStyle = dwStyle | CBS_DROPDOWNLIST;
			break;
		}
	}

	RetVal = CComboBox::Create( dwStyle, ArxRect, pParentWnd, nID );

	LimitText(pControl->GetLongProperty(Prop::LimitText));
	m_ToolTip.Create(this);
	SetToolTipEx(this, m_ToolTip, pControl);

	SetExtendedUI(TRUE);
	switch (m_ArxControl->GetLongProperty(Prop::EventInvoke))
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
			m_ArxControl->GetStrProperty(Prop::EventMouseMove),
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
		CString sTestText = m_ArxControl->GetStrProperty(Prop::Text);
		if (sTestText != sText && m_ArxControl)
		{
			m_ArxControl->SetStringProperty(Prop::Text, sText);
			// call methods to invoke the event
			InvokeMethodString(m_ArxControl->GetStrProperty(Prop::EventEditChanged), sText, m_bInvokeWithSendString);
		}
	}
	//else
	//{
	//	// Send Notification to parent of ListView ctrl
	//	CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();
	//	pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sText);	
	//	
	//	// fire the on Grid edit cell event.
	//	pListCtrl->EndEditControls(pListCtrl);
	//}
	
	// if we are not to auto update the text, get outta here
	if (!m_bAutoComplete) 
		return;

	// Get the text in the edit box
	CString str;
	GetWindowText(str);
	CString sLBText;
	if( GetCurSel() >= 0 )
	{
		GetLBText( GetCurSel(), sLBText );
		if( sLBText == str )
			return; //no-op
	}
	int cchText = str.GetLength();

	// Currently selected range
	DWORD dwCurSel = GetEditSel();
	WORD dStart = LOWORD(dwCurSel);
	WORD dEnd   = HIWORD(dwCurSel);

	for( int idx = 0; idx < GetCount(); ++idx )
	{
		GetLBText( idx, sLBText );
		if( sLBText.Left( cchText ).CompareNoCase( str ) == 0 )
		{
			m_bAutoComplete = false;
			SetCurSel( idx );
			m_bAutoComplete = true;
			SetEditSel(cchText, -1);
		}
	}
}


void VdclComboBox::OnKillfocus() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(Prop::EventKillFocus), m_bInvokeWithSendString);

}

void VdclComboBox::OnSetfocus() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(Prop::EventSetFocus), m_bInvokeWithSendString);

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

			CString sText = m_ArxControl->GetStrProperty(Prop::EventSelChanged);

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
			
			InvokeMethodIntString(m_ArxControl->GetStrProperty(Prop::EventSelChanged), nSel, sString, m_bInvokeWithSendString);
			m_ArxControl->SetStringProperty(Prop::Text, sString);
		
		}
	}
	
	//if (m_ArxControl == NULL)
	//{		
	//	// Send Notification to parent of ListView ctrl
	//	CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();
	//	pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sString);	
	//	
	//	if (m_pGridEdit != NULL)
	//	{
	//		m_pGridEdit->SetWindowText(sString);
	//	}
	//	// fire the on Grid edit cell event.
	//	pListCtrl->EndEditControls(pListCtrl);
	//}
}

//
//
//void VdclComboBox::OnShowWindow(BOOL bShow, UINT nStatus) 
//{
//	if (m_ArxControl)
//	{
//		if (m_ArxControl->GetLongProperty(Prop::ComboBoxStyle) == CmboStyle_Simple)
//		{		
// 			CRect rcThis;
//			GetWindowRect(&rcThis);
//			GetParent()->ScreenToClient(rcThis);
//			
//			CRect rcListBox;
//			GetClientRect(rcListBox);
//			rcThis.bottom = rcThis.bottom -1;
//
//			MoveWindow(rcThis, TRUE);
//		}
//	}
//	
//	CComboBox::OnShowWindow(bShow, nStatus);	
//}

BOOL VdclComboBox::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	
	if (pMsg->message == WM_KEYDOWN)
	{
		m_bAutoComplete = true;
		WPARAM nVirtKey = pMsg->wParam;
		if (nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
			m_bAutoComplete = false;
		if (m_ArxControl)
		{
			if (nVirtKey == VK_RETURN && m_ArxControl->GetBooleanProperty(Prop::ReturnAsTab) == TRUE)
				pMsg->wParam = VK_TAB;		
		}
		else
		{
			if (nVirtKey == VK_RETURN || nVirtKey == VK_ESCAPE)
			{
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				m_bESC = pMsg->wParam == VK_ESCAPE;
				return TRUE;				// DO NOT process further
			}
		}
	}
	return CComboBox::PreTranslateMessage(pMsg);
}



void VdclComboBox::OnDropdown() 
{

	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(Prop::EventDropDown), m_bInvokeWithSendString);	
}
