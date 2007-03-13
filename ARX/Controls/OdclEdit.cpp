// OdclEdit.cpp : implementation file
//

#include "stdafx.h"
#include "OdclEdit.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "ArxGridCtrl.h"
#include "PropertyIds.h"

const TCHAR sAllChars[] = _T("*");


/////////////////////////////////////////////////////////////////////////////
// OdclEdit

OdclEdit::OdclEdit()
{
	m_sFilter = CString();	
	m_bAllowReturn = false;
	m_bFocusClick = false;
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;
	m_pGridDropList = NULL;
}

OdclEdit::~OdclEdit()
{
}


BEGIN_MESSAGE_MAP(OdclEdit, CColorEdit)
	//{{AFX_MSG_MAP(OdclEdit)
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_CONTROL_REFLECT(EN_ERRSPACE, OnErrspace)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_MAXTEXT, OnMaxtext)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OdclEdit message handlers
BOOL OdclEdit::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
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
	
	dwStyle = WS_CHILD | WS_VISIBLE | ES_WANTRETURN | WS_CLIPSIBLINGS;
	//	
	
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	if (pControl->GetBoolProperty(nHScrollBar) == TRUE)
		dwStyle = dwStyle | WS_HSCROLL;
	if (pControl->GetBoolProperty(nVScrollBar) == TRUE)
		dwStyle = dwStyle | WS_VSCROLL;
	if (m_ArxControl->GetBoolProperty(nReadOnly) == TRUE)
		dwStyle = dwStyle | ES_READONLY;
		
	switch (m_ArxControl->GetLngProperty(nFilterStyle))
	{
	case 2: /* Integer */
		dwStyle = dwStyle | ES_NUMBER | ES_AUTOHSCROLL;
		break;
	case 5:/*Upper case*/
		dwStyle = dwStyle | ES_UPPERCASE | ES_AUTOHSCROLL;
		break;
	case 6:/*lower case*/
		dwStyle = dwStyle | ES_LOWERCASE | ES_AUTOHSCROLL;
		break;
	case 7:/*password*/
		dwStyle = dwStyle | ES_PASSWORD | ES_AUTOHSCROLL;
		break;
	case 8:/*MultiLine*/
		dwStyle = dwStyle | ES_MULTILINE | ES_AUTOVSCROLL;
		m_bAllowReturn = true;
		break;
	default:
		dwStyle = dwStyle | ES_AUTOHSCROLL;
		break;
	}

	
	// adjust the justification style				
	switch (m_ArxControl->GetLngProperty(nJustification))
	{
	case 0:/*Left*/
		dwStyle = dwStyle | ES_LEFT;
		break;
	case 1:/*Center*/
		dwStyle = dwStyle | ES_CENTER;
		break;
	case 2:/*Right*/
		dwStyle = dwStyle | ES_RIGHT;
		break;
	}

	RetVal=  CColorEdit::Create( dwStyle, ArxRect, pParentWnd, nID );
	

	
	VERIFY(CColorEdit::SubclassDlgItem(nID, pParentWnd));
	
	m_pTextProp = pControl->GetPropertyObject(nText);
	
	CColorEdit::SetLimitText(pControl->GetLngProperty(nLimitText));
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


BOOL OdclEdit::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;
	
	// set the arx control pointer
    m_ArxControl = NULL;
	
	dwStyle = WS_CHILD | WS_VISIBLE | ES_WANTRETURN | WS_CLIPSIBLINGS;
	
	dwStyle = dwStyle | WS_TABSTOP;
	dwStyle = dwStyle | WS_GROUP;

	if (nStyle == 8)
	{
		dwStyle = dwStyle | WS_BORDER;
		dwStyle = dwStyle | WS_HSCROLL;
		dwStyle = dwStyle | WS_VSCROLL;
	}
	
	switch (nStyle)
	{
	case 2: /* Integer */
		dwStyle = dwStyle | ES_NUMBER | ES_AUTOHSCROLL;
		break;
	case 5:/*Upper case*/
		dwStyle = dwStyle | ES_UPPERCASE | ES_AUTOHSCROLL;
		break;
	case 6:/*lower case*/
		dwStyle = dwStyle | ES_LOWERCASE | ES_AUTOHSCROLL;
		break;
	case 7:/*password*/
		dwStyle = dwStyle | ES_PASSWORD | ES_AUTOHSCROLL;
		break;
	case 8:/*MultiLine*/
		dwStyle = dwStyle | ES_MULTILINE | ES_AUTOVSCROLL;
		m_bAllowReturn = true;
		break;
	default:
		dwStyle = dwStyle | ES_AUTOHSCROLL;
		break;
	}

	
	dwStyle = dwStyle | ES_LEFT;
	//dwStyle = dwStyle | ES_CENTER;
	//dwStyle = dwStyle | ES_RIGHT;

	RetVal=  CColorEdit::Create( dwStyle, rc, pParentWnd, nID );
		
	VERIFY(CColorEdit::SubclassDlgItem(nID, pParentWnd));
		
	return RetVal;
	
}

void OdclEdit::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (m_ArxControl)
		InvokeMethodIntIntInt(
			m_ArxControl->GetStrProperty(nEventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);	
	
	CColorEdit::OnMouseMove(nFlags, point);
}
void OdclEdit::OnDestroy() 
{
	
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
	CColorEdit::OnDestroy();
	
}

void OdclEdit::OnChange() 
{
		
	CString sText;	
	GetWindowText(sText);

	
	if (m_ArxControl)
	{		
		CString sTestText = m_ArxControl->GetStrProperty(nText);
		if (sTestText != sText)
		{
			//AfxMessageBox(m_ArxControl->GetStrProperty(nEventEditChanged));
			m_ArxControl->SetStringProperty(nText, sText);
			// call methods to invoke the event
			InvokeMethodString(m_ArxControl->GetStrProperty(nEventEditChanged), sText, m_bInvokeWithSendString);
		}
	}

	if (m_ArxControl == NULL)
	{
		// Send Notification to parent of ListView ctrl
		CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent();

		//pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, GetCurrentItemColorIndex());
		pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sText);
	}

}

void OdclEdit::OnErrspace() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventOutOfMemory), m_bInvokeWithSendString);
	
}

void OdclEdit::OnKillfocus() 
{
	m_bFocusClick = false;

	if (m_ArxControl)
	{
		if (m_ArxControl->m_bEventsAsAction)
		{
			GetParent()->GetParent()->EnableWindow(TRUE);
			int stat;
			struct resbuf *result = NULL, *list;    

			CString str;
			GetWindowText(str);
      struct resbuf *VarVal = acutBuildList(RTSTR, (LPCTSTR)str, 0);
			stat = acedPutSym(_T("$value"), VarVal);
      acutRelRb(VarVal);

			CString sText = m_ArxControl->GetStrProperty(nEventEditChanged);

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
			if (GetParent()->IsWindowVisible() && m_ArxControl)
			// call methods to invoke the event
			InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
		}
	}
/*
	if (m_ArxControl == NULL)
	{
		CString sText;
		GetWindowText(sText);
		// Send Notification to parent of ListView ctrl
		LV_DISPINFO dispinfo;
		dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
		dispinfo.hdr.idFrom = GetDlgCtrlID();
		dispinfo.hdr.code = LVN_ENDLABELEDIT;

		CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent();

		pListCtrl->m_bShowHighlight = true;
		
		//pListCtrl->SetItemText(
		//	pListCtrl->m_nRowSelected, 
		//	pListCtrl->m_nColSelected, sText);
		
		dispinfo.item.mask = LVIF_TEXT;
		dispinfo.item.iItem = pListCtrl->m_nRowSelected;
		dispinfo.item.iSubItem = pListCtrl->m_nColSelected;
		dispinfo.item.pszText = LPTSTR((LPCTSTR)sText);
		dispinfo.item.cchTextMax = sText.GetLength();

		pListCtrl->SendMessage( WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );
		
		pListCtrl->HideEditControls();
		ShowWindow(FALSE);
	}
	*/
}

void OdclEdit::OnMaxtext() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventMaxText), m_bInvokeWithSendString);
	
}

void OdclEdit::OnSetfocus() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
	m_bFocusClick = false;
}


void OdclEdit::OnUpdate() 
{
	if (m_ArxControl)
	{
		CString sText;
		
		GetWindowText(sText);
		if (IsWindowVisible() == TRUE)
			// call methods to invoke the event
			InvokeMethodString(m_ArxControl->GetStrProperty(nEventUpdate), sText, m_bInvokeWithSendString);

	}
}


void OdclEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_ArxControl)
		InvokeMethodStringIntInt(m_ArxControl->GetStrProperty(nEventKeyUp), CString((char)nChar), (int)nRepCnt,  (int)nFlags, m_bInvokeWithSendString);
	CColorEdit::OnKeyUp(nChar, nRepCnt, nFlags);

}

void OdclEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_ArxControl)
		InvokeMethodStringIntInt(m_ArxControl->GetStrProperty(nEventKeyDown), CString((char)nChar),  (int)nRepCnt, (int)nFlags, m_bInvokeWithSendString);
	
	if (m_ArxControl == NULL && !m_bAllowReturn)
	{
		CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent();

		if (nChar == VK_UP || nChar == VK_DOWN)
		{
			if (m_pGridDropList == NULL)
			{
				if (nChar == VK_UP)
					pListCtrl->MoveUp();		
				
				if (nChar == VK_DOWN)
					pListCtrl->MoveDown();					
			}
			else
			{
				CString sText;
				GetWindowText(sText);

				int n = m_pGridDropList->FindString(0, sText);
				if (n > -1)
				{
					if (nChar == VK_UP)
						n--;
					if (nChar == VK_DOWN)
						n++;
					
					if (n < 0)
						n = m_pGridDropList->GetCount()-1;

					if (n > m_pGridDropList->GetCount()-1)
						n = 0;
				}
				else
				{
					if (nChar == VK_UP)
						n = m_pGridDropList->GetCount()-1;			
					if (nChar == VK_DOWN)
						n = 0;
				}

				m_pGridDropList->GetLBText(n, sText);
				SetWindowText(sText);
				SetSel(0, sText.GetLength(), TRUE);
			}
		}
	}

	CColorEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void OdclEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	CString strChar = sChar;
	

	if (nChar == VK_ESCAPE && m_ArxControl == NULL)
		return;

	if (nChar == 8)
	{
		CColorEdit::OnChar(nChar, nRepCnt, nFlags);
		return;
	}

	if (m_pGridDropList != NULL && !m_bAllowReturn)
	{
		CString sText;

		if (m_sFilter.GetLength() > 0)
		{
			if (m_sFilter.FindOneOf(strChar) <= -1)
			{
				OnBadInput();
				return;
			}
		}

		CColorEdit::OnChar(nChar, nRepCnt, nFlags);
		GetWindowText(sText);

		int n = m_pGridDropList->FindString(0, sText);
		if (n > -1)
		{
			int nLen = sText.GetLength();
			m_pGridDropList->GetLBText(n, sText);
			SetWindowText(sText);
			SetSel(nLen, sText.GetLength(), TRUE);
			return;
		}
		return;
	}

	if (m_sFilter.GetLength() == 0 ||
		m_sFilter == sAllChars)
	{
		CColorEdit::OnChar(nChar, nRepCnt, nFlags);
		return;
	}
	
	if (m_sFilter.FindOneOf(strChar) > -1)
		CColorEdit::OnChar(nChar, nRepCnt, nFlags);
	else
	{
		OnBadInput();
		return;
	}

}
void OdclEdit::OnBadInput()
{
	MessageBeep((UINT)-1);
}

void OdclEdit::SetTooltipText(CString* spText, BOOL bActivate)
{
} // End of SetTooltipText
void OdclEdit::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
		m_ToolTip.Create(this);
} // End of InitToolTip

BOOL OdclEdit::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN && m_ArxControl && !m_bAllowReturn)
	{
		CString sEvent = m_ArxControl->GetStrProperty(nEventReturnPressed);
		InvokeMethod(sEvent, m_bInvokeWithSendString);	

		if (m_ArxControl->GetBoolProperty(nReturnAsTab) == TRUE)
		{
			pMsg->wParam = VK_TAB;
		}
		else
		{
			if (sEvent.GetLength() > 0 && !m_bAllowReturn)
			{
				pMsg->wParam = NULL;
				pMsg->message = NULL;
			}
		}
	}
	if (pMsg->message== WM_KEYDOWN && m_ArxControl == NULL && !m_bAllowReturn)
	{
		if (pMsg->wParam == VK_RETURN && !m_bAllowReturn)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent();		
			pListCtrl->MoveDown();		
			return TRUE;				// DO NOT process further
		}
		if (pMsg->wParam == VK_ESCAPE)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			SetWindowText(m_strOldValue);
			
			// Send Notification to parent of ListView ctrl
			CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent();

			pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, m_strOldValue);
			pListCtrl->HideEditControls();
			return TRUE;				// DO NOT process further
		}
		
	}


	return CColorEdit::PreTranslateMessage(pMsg);
}

void OdclEdit::SetDragnDrop(BOOL bRegister)
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

void OdclEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_ArxControl)
	{
		if (m_ArxControl->GetBoolProperty(nDragnDropAllowBegin) == TRUE && nFlags == 1)
		{
			BeginDragnDrop(m_ArxControl, point, m_bInvokeWithSendString);
		}
	}
	
	CColorEdit::OnLButtonDown(nFlags, point);
}



void OdclEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CColorEdit::OnLButtonUp(nFlags, point);
	if (!m_bFocusClick && m_ArxControl)
	{
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
		m_bFocusClick = true;
	}
}
