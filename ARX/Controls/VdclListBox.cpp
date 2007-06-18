// VdclListBox.cpp : implementation file
//

#include "stdafx.h"
#include "VdclListBox.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// VdclListBox

VdclListBox::VdclListBox()
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

}

VdclListBox::~VdclListBox()
{
}


BEGIN_MESSAGE_MAP(VdclListBox, CClrListBox)
	//{{AFX_MSG_MAP(VdclListBox)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblclk)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetfocus)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
  ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclListBox message handlers

BOOL VdclListBox::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS
		| LBS_HASSTRINGS | LBS_NOTIFY | LBS_DISABLENOSCROLL | LBS_USETABSTOPS;
	
	if (pControl->GetBoolProperty(nNoIntegralHeight) == TRUE)
		dwStyle = dwStyle | LBS_NOINTEGRALHEIGHT;
	
	if (pControl->GetBoolProperty(nMultiColumn) == TRUE)
		dwStyle = dwStyle | LBS_MULTICOLUMN;
	
	if (pControl->GetBoolProperty(nSorted) == TRUE)
		dwStyle = dwStyle | LBS_SORT;		

	// get the selection style and apply the appropriate setting
	switch (pControl->GetLngProperty(nSelectStyle))
	{
	case 1:
		dwStyle = dwStyle | LBS_EXTENDEDSEL;
		break;
	case 2:
		dwStyle = dwStyle | LBS_MULTIPLESEL;
		break;
	}

	if (pControl->GetBoolProperty(nVScrollBar) == TRUE)
		dwStyle = dwStyle | WS_VSCROLL;	

	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	BOOL RetVal = CClrListBox::Create(dwStyle,ArxRect, pParentWnd, nID);

	// fix up 3D styles
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	CClrListBox::ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
	
	ModifyStyleEx(0, WS_VSCROLL);
	CClrListBox::ModifyStyleEx(0, WS_VSCROLL);
	
	m_ToolTip.Create(this);
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

int VdclListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CClrListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	RefCountedPtr< CPropertyObject > pListProperty = m_ArxControl->GetPropertyObject(nList);

	for (size_t i = 0; i < pListProperty->size(); i++)
		CClrListBox::AddString(pListProperty->GetStringItem(i));

	return 0;
}

void VdclListBox::OnDestroy() 
{
	CClrListBox::OnDestroy();
}

void VdclListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
CString sEventName = m_ArxControl->GetStrProperty(nEventMouseMove);

	InvokeMethodIntIntInt(
		sEventName,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	
	CClrListBox::OnMouseMove(nFlags, point);
}
void VdclListBox::GetCurrentSelection() 
{
	int nSelCount = CClrListBox::GetSelCount();
	
	if (nSelCount > -1)
	{
		// Get the indexes of all the selected items.
		int nCount = CClrListBox::GetSelCount();
		CArray<int,int> aryListBoxSel;

		// get an array of all the selected line numbers
		aryListBoxSel.SetSize(nCount);
		CClrListBox::GetSelItems(nCount, aryListBoxSel.GetData());
		CStringList sSelList;
		
		for (int i=0; i<nCount; i++)
		{
			long nLineTagId = CClrListBox::GetItemData(aryListBoxSel[i]);

			// get the text name of the selected line number
			CString sTextItem;
			int n = CClrListBox::GetTextLen(aryListBoxSel[i]);
			CClrListBox::GetText(aryListBoxSel[i], sTextItem.GetBuffer(n));
			sTextItem.ReleaseBuffer();

			sSelList.AddTail(sTextItem);
		}


		// call methods to invoke the event
		InvokeMethodIntList(m_ArxControl->GetStrProperty(nEventSelChanged), nSelCount, &sSelList, m_bInvokeWithSendString);
	}   
		
	if (nSelCount == -1)
	{
		nSelCount = CClrListBox::GetCurSel();
	
		CString sSelText;
		
		int nIndex = CClrListBox::GetCurSel();
		if (nIndex > -1)
		{
			int n = CClrListBox::GetTextLen(nIndex);      
			CClrListBox::GetText(nIndex, sSelText.GetBuffer(n));
			sSelText.ReleaseBuffer();
		}

		// call methods to invoke the event
		InvokeMethodIntString(m_ArxControl->GetStrProperty(nEventSelChanged), nSelCount, sSelText, m_bInvokeWithSendString);
	}   
}	

void VdclListBox::OnSelchange() 
{
	int nSelCount = GetSelCount();
	
	if (m_ArxControl->m_bEventsAsAction)
	{
		int nCurSel = GetCurSel();
		GetParent()->GetParent()->EnableWindow(TRUE);
		int stat;
		struct resbuf *result = NULL, *list;    
		CString sVal;
		sVal.Format(_T("%d"), nCurSel);
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
		if (nSelCount > -1)
		{
			// call methods to invoke the event
			InvokeMethodIntString(m_ArxControl->GetStrProperty(nEventSelChanged), nSelCount, CString(), m_bInvokeWithSendString);
		}   

		if (nSelCount == -1)
		{
			int nCurSel = GetCurSel();
		
			CString sSelText;
			
			if (nCurSel > -1)
			{
				int n = GetTextLen(nCurSel);      
				GetText(nCurSel, sSelText.GetBuffer(n));
				sSelText.ReleaseBuffer();
			}

			// call methods to invoke the event
			InvokeMethodIntString(m_ArxControl->GetStrProperty(nEventSelChanged), nCurSel, sSelText, m_bInvokeWithSendString);
		}
	}	   
}

void VdclListBox::OnDblclk() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventDblClicked), m_bInvokeWithSendString);
}

void VdclListBox::OnKillfocus() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
}

void VdclListBox::OnSetfocus() 
{
	// call methods to invoke the event
	InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);	
}

BOOL VdclListBox::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CClrListBox::PreTranslateMessage(pMsg);
}

void VdclListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if (nChar == VK_RETURN) {
			//Change return into a double-click
			InvokeMethod(m_ArxControl->GetStrProperty(nEventDblClicked), m_bInvokeWithSendString);
  } else {
    CClrListBox::OnKeyDown(nChar, nRepCnt, nFlags);
  }
}

void VdclListBox::SetDragnDrop(BOOL bRegister)
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

void VdclListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CClrListBox::OnLButtonDown(nFlags, point);

	// Get the currently selected Item
    if(GetCurSel() == LB_ERR)
        return;
    
	if (m_ArxControl->GetBoolProperty(nDragnDropAllowBegin) == TRUE)
	{
		int nCount = GetSelCount();
		int nIndex = -1;
		
		BeginDragnDrop(m_ArxControl, point, m_bInvokeWithSendString);
		
		// We need to send WM_LBUTTONUP to control or else the selection rectangle 
		// will "follow" the mouse (like when you hold the left mouse down and 
		// scroll through a regular listbox). WM_LBUTTONUP was sent to window that 
		// recieved the drag/drop, not the one that initiated it, so we simulate
		// an WM_LBUTTONUP to the initiating window.
		LPARAM lparam;
    
		// "Pack" lparam with x and y coordinates where lbuttondown originated
		lparam=point.y; 
		lparam=lparam<<16;
		lparam |= point.x;                         
    
		SendMessage(WM_LBUTTONUP,0,lparam);	
	}
}

void VdclListBox::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CString sEventName = m_ArxControl->GetStrProperty(nEventRClick);

	if (sEventName.GetLength() > 0)
	{
		// do a hit test to see if the user has right clicked on a list item.
		for (int i=0; i<GetCount(); i++)
		{
			CRect rc;
			GetItemRect(i, rc);

			// if the point is in the rect of this item.
			if (rc.PtInRect(point))
			{
				// select the item
				CListBox::SetCurSel(i);
				break;
			}
		}
		// call methods to invoke the event
		InvokeMethod(sEventName, m_bInvokeWithSendString);
	}
	CListBox::OnRButtonUp(nFlags, point);
}
