// ArxListBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxListBoxCtrl.h"
#include "PropertyObject.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CArxListBoxCtrl

CArxListBoxCtrl::CArxListBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CListBoxCtrl( pTemplate, pPane, nID, false )
, mArxServices( pTemplate )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxListBoxCtrl::~CArxListBoxCtrl()
{
}

bool CArxListBoxCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	if( GetTemplate()->GetLongProperty(Prop::EventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}

bool CArxListBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::DragnDropAllowDrop:
		{
			SetDragnDrop( pProp->GetBooleanValue() );
			break;
		}
	}
	return !bFailed;
}

void CArxListBoxCtrl::GetCurrentSelection() 
{
	int nSelCount = CListBoxCtrl::GetSelCount();
	
	if (nSelCount > -1)
	{
		// Get the indexes of all the selected items.
		CArray<int,int> aryListBoxSel;
		aryListBoxSel.SetSize(nSelCount);
		CListBoxCtrl::GetSelItems(nSelCount, aryListBoxSel.GetData());
		CStringList sSelList;
		for (int i=0; i<nSelCount; i++)
		{
			CString sTextItem;
			GetText(aryListBoxSel[i], sTextItem);
			sSelList.AddTail(sTextItem);
		}
		// call methods to invoke the event
		InvokeMethodIntList(mpTemplate->GetStringProperty(Prop::EventSelChanged), nSelCount, &sSelList, m_bInvokeWithSendString);
	}   
	else if (nSelCount == -1)
	{
		nSelCount = CListBoxCtrl::GetCurSel();
	
		CString sSelText;
		int nIndex = CListBoxCtrl::GetCurSel();
		if (nIndex > -1)
			GetText(nIndex, sSelText);

		// call methods to invoke the event
		InvokeMethodIntString(mpTemplate->GetStringProperty(Prop::EventSelChanged), nSelCount, sSelText, m_bInvokeWithSendString);
	}   
}	

void CArxListBoxCtrl::SetDragnDrop(BOOL bRegister)
{
	if (bRegister == TRUE)
	{
		BOOL success = m_DropTarget.Register(this);
		m_DropTarget.m_pThisArxControl = mpTemplate;
		m_DropTarget.m_pParent = this;
	}
	else
		m_DropTarget.Revoke();
}


BEGIN_MESSAGE_MAP(CArxListBoxCtrl, CListBoxCtrl)
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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArxListBoxCtrl message handlers

void CArxListBoxCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
CString sEventName = mpTemplate->GetStringProperty(Prop::EventMouseMove);

	InvokeMethodIntIntInt(
		sEventName,
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	
	
	CListBoxCtrl::OnMouseMove(nFlags, point);
}

void CArxListBoxCtrl::OnSelchange() 
{
	int nSelCount = GetSelCount();
	
	if (mpTemplate->m_bEventsAsAction)
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

    CString sText = mpTemplate->GetStringProperty(Prop::EventSelChanged);

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
			InvokeMethodIntString(mpTemplate->GetStringProperty(Prop::EventSelChanged), nSelCount, CString(), m_bInvokeWithSendString);
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
			InvokeMethodIntString(mpTemplate->GetStringProperty(Prop::EventSelChanged), nCurSel, sSelText, m_bInvokeWithSendString);
		}
	}	   
}

void CArxListBoxCtrl::OnDblclk() 
{
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), m_bInvokeWithSendString);
}

void CArxListBoxCtrl::OnKillfocus() 
{
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventKillFocus), m_bInvokeWithSendString);
}

void CArxListBoxCtrl::OnSetfocus() 
{
	// call methods to invoke the event
	InvokeMethod(mpTemplate->GetStringProperty(Prop::EventSetFocus), m_bInvokeWithSendString);	
}

BOOL CArxListBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return CListBoxCtrl::PreTranslateMessage(pMsg);
}

void CArxListBoxCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if (nChar == VK_RETURN) {
			//Change return into a double-click
			InvokeMethod(mpTemplate->GetStringProperty(Prop::EventDblClicked), m_bInvokeWithSendString);
  } else {
    CListBoxCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
  }
}

void CArxListBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CListBoxCtrl::OnLButtonDown(nFlags, point);

	// Get the currently selected Item
  if(GetCurSel() == LB_ERR)
      return;
    
	if (mpTemplate->GetBooleanProperty(Prop::DragnDropAllowBegin) == TRUE)
	{
		DWORD dwDropEffect = BeginDragnDrop(mpTemplate, point, m_bInvokeWithSendString);
		OnSelchange();

		// We need to send WM_LBUTTONUP to control or else the selection rectangle 
		// will "follow" the mouse (like when you hold the left mouse down and 
		// scroll through a regular listbox). WM_LBUTTONUP was sent to window that 
		// recieved the drag/drop, not the one that initiated it, so we simulate
		// an WM_LBUTTONUP to the initiating window.
		if( dwDropEffect != DROPEFFECT_NONE )
			SendMessage(WM_LBUTTONUP,0,MAKELPARAM(point.x,point.y));	
	}
}

void CArxListBoxCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CString sEventName = mpTemplate->GetStringProperty(Prop::EventRClick);
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
				CListBoxCtrl::SetCurSel(i);
				break;
			}
		}
		// call methods to invoke the event
		InvokeMethod(sEventName, m_bInvokeWithSendString);
	}
	CListBoxCtrl::OnRButtonUp(nFlags, point);
}
