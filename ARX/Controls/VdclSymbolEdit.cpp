// VdclSymbolEdit.cpp : implementation file
//

#include "stdafx.h"
#include "VdclSymbolEdit.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "SpreadSheet.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// VdclSymbolEdit

VdclSymbolEdit::VdclSymbolEdit()
{
	m_bESC = false;
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

	m_bFocusClick = false;	
	m_pGridDropList = NULL;
}

VdclSymbolEdit::~VdclSymbolEdit()
{
}


BEGIN_MESSAGE_MAP(VdclSymbolEdit, CAcUiSymbolEdit)
	//{{AFX_MSG_MAP(VdclSymbolEdit)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_CONTROL_REFLECT(EN_ERRSPACE, OnErrspace)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_MAXTEXT, OnMaxtext)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclSymbolEdit message handlers


BOOL VdclSymbolEdit::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
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
	

	dwStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOHSCROLL;
	//WS_CLIPCHILDREN | 
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	// adjust the justification style				
	switch (m_ArxControl->GetLngProperty(nJustification))
	{
	case 0:/*Left*/
		dwStyle = dwStyle | ES_LEFT | WS_CLIPSIBLINGS;
		break;
	case 1:/*Center*/
		dwStyle = dwStyle | ES_CENTER;
		break;
	case 2:/*Right*/
		dwStyle = dwStyle | ES_RIGHT;
		break;
	}

	RetVal=  CAcUiSymbolEdit::Create( dwStyle, ArxRect, pParentWnd, nID );
	
	VERIFY(CAcUiSymbolEdit::SubclassDlgItem(nID, pParentWnd));
	// fix up 3D styles
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	CAcUiSymbolEdit::ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);

	m_pTextProp = pControl->GetPropertyObject(nText);
	CAcUiSymbolEdit::SetLimitText(pControl->GetLngProperty(nLimitText));

	InitToolTip();
	SetToolTipEx(this, m_ToolTip, pControl);

	//SetTooltipText(
//		&pControl->GetStrProperty(nToolTipText), 
//		TRUE);

	switch (m_ArxControl->GetLngProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}

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


BOOL VdclSymbolEdit::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
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
		dwStyle = dwStyle | WS_HSCROLL;
		dwStyle = dwStyle | WS_VSCROLL;
	}
	
	dwStyle = dwStyle | ES_AUTOHSCROLL;
	
	dwStyle = dwStyle | ES_LEFT;
	//dwStyle = dwStyle | ES_CENTER;
	//dwStyle = dwStyle | ES_RIGHT;

	RetVal = CAcUiSymbolEdit::Create( dwStyle, rc, pParentWnd, nID );
		
	VERIFY(CAcUiSymbolEdit::SubclassDlgItem(nID, pParentWnd));
		
	return RetVal;
	
}

void VdclSymbolEdit::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
	CAcUiSymbolEdit::OnDestroy();	
	
}

void VdclSymbolEdit::OnChange() 
{
	CString sText;
	VdclSymbolEdit::GetWindowText(sText);

	if (m_ArxControl)
	{
		CString sTestText = m_ArxControl->GetStrProperty(nText);
		if (sTestText != sText)
		{
			m_ArxControl->SetStringProperty(nText, sText);
			// call methods to invoke the event
			InvokeMethodString(m_ArxControl->GetStrProperty(nEventEditChanged), sText, m_bInvokeWithSendString);
		}
	}
	
	if (m_ArxControl == NULL)
	{
		// Send Notification to parent of ListView ctrl
		LV_DISPINFO dispinfo;
		dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
		dispinfo.hdr.idFrom = GetDlgCtrlID();
		dispinfo.hdr.code = LVN_ENDLABELEDIT;

		CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent();

		dispinfo.item.mask = LVIF_TEXT;
		dispinfo.item.iItem = pListCtrl->m_nRowSelected;
		dispinfo.item.iSubItem = pListCtrl->m_nColSelected;
		dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)sText);
		dispinfo.item.cchTextMax = sText.GetLength();

		pListCtrl->SendMessage( WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );
	}
}


void VdclSymbolEdit::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (m_ArxControl)
		InvokeMethodIntIntInt(
			m_ArxControl->GetStrProperty(nEventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
	
	
	CAcUiSymbolEdit::OnMouseMove(nFlags, point);
}
void VdclSymbolEdit::OnErrspace() 
{
	// call methods to invoke the event
	if (m_ArxControl)
		InvokeMethod(m_ArxControl->GetStrProperty(nEventOutOfMemory), m_bInvokeWithSendString);
	
}

void VdclSymbolEdit::OnKillfocus() 
{
	CString ThisText;
	
	// Get and update text the user typed in.
	CAcUiSymbolEdit::GetWindowText(ThisText);	
	
	if (CAcUiSymbolEdit::Validate())
		// if the value is valid then update old text string
		m_strOldValue = ThisText;
	else
		// if the value is not valid then reset the current string
		ThisText = m_strOldValue;

	CAcUiSymbolEdit::Convert();	
	CAcUiSymbolEdit::SetWindowText(ThisText);
	
	if (GetParent()->IsWindowVisible() && m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
	
}

void VdclSymbolEdit::OnMaxtext() 
{
	// call methods to invoke the event
	if (m_ArxControl)
		InvokeMethod(m_ArxControl->GetStrProperty(nEventMaxText), m_bInvokeWithSendString);
	
}

void VdclSymbolEdit::OnSetfocus() 
{
	// call methods to invoke the event
	if (m_ArxControl)
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
	
}

void VdclSymbolEdit::OnUpdate() 
{
	CString sText;
	
	GetWindowText(sText);
	
	if (IsWindowVisible() == TRUE)
		// call methods to invoke the event
		if (m_ArxControl)
			InvokeMethodString(m_ArxControl->GetStrProperty(nEventUpdate), sText, m_bInvokeWithSendString);
	
}


void VdclSymbolEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	if (m_ArxControl)
		InvokeMethodStringIntInt(m_ArxControl->GetStrProperty(nEventKeyUp), sChar, nRepCnt, nFlags, m_bInvokeWithSendString);
	
	
	CAcUiSymbolEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}

void VdclSymbolEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	if (m_ArxControl)
		InvokeMethodStringIntInt(m_ArxControl->GetStrProperty(nEventKeyDown), sChar, nRepCnt, nFlags, m_bInvokeWithSendString);
		
	if (m_ArxControl == NULL)
	{
		CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent();
			
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

	CAcUiSymbolEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void VdclSymbolEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE && m_ArxControl == NULL)
		return;

	if (m_pGridDropList != NULL)
	{
		CString sText;
		CEdit::OnChar(nChar, nRepCnt, nFlags);
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
	}

	if (nChar == 13)
	{
		if (m_ArxControl)
			InvokeMethod(m_ArxControl->GetStrProperty(nEventReturnPressed), m_bInvokeWithSendString);
		return;
	}
	
	CAcUiSymbolEdit::OnChar(nChar, nRepCnt, nFlags);
}

void VdclSymbolEdit::SetTooltipText(CString* spText, BOOL bActivate)
{
	/*
	// We cannot accept NULL pointer
	if (spText == NULL) return;

	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, (LPCTSTR)*spText, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip.UpdateTipText((LPCTSTR)*spText, this, 1);
	m_ToolTip.Activate(bActivate);
	*/
} // End of SetTooltipText
void VdclSymbolEdit::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip

BOOL VdclSymbolEdit::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		if (m_ArxControl)
		{
			CString sEvent = m_ArxControl->GetStrProperty(nEventReturnPressed);
			InvokeMethod(sEvent, m_bInvokeWithSendString);	

			if (m_ArxControl->GetBoolProperty(nReturnAsTab) == TRUE)
			{
				pMsg->wParam = VK_TAB;
			}
			else
			{
				if (sEvent.GetLength() > 0)
				{
					pMsg->wParam = NULL;
					pMsg->message = NULL;
				}
			}
		}
	}
	if (pMsg->message== WM_KEYDOWN && m_ArxControl == NULL)
	{
		if (pMsg->wParam == VK_RETURN)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent();		
			pListCtrl->MoveDown();		
			return TRUE;				// DO NOT process further
		}
		if (pMsg->wParam == VK_ESCAPE)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			SetWindowText(m_strOldValue);
			
			// Send Notification to parent of ListView ctrl
			CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent();
			pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, m_strOldValue);			
			pListCtrl->HideEditControls();
			return TRUE;				// DO NOT process further
		}
		
	}

	return CAcUiSymbolEdit::PreTranslateMessage(pMsg);
}
void VdclSymbolEdit::SetDragnDrop(BOOL bRegister)
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


void VdclSymbolEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	
	CAcUiSymbolEdit::OnLButtonDown(nFlags, point);
}


void VdclSymbolEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CAcUiSymbolEdit::OnLButtonUp(nFlags, point);
	if (!m_bFocusClick)
	{
		if (m_ArxControl)
			InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
		m_bFocusClick = true;
	}

}
