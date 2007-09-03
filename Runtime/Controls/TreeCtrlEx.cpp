// TreeCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "TreeCtrlEx.h"
#include "InvokeMethod.h"
#include "VdclTree.h"
#include "PropertyIds.h"
#include "DclControlObject.h"


/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx

CTreeCtrlEx::CTreeCtrlEx()
{
	m_ToolTip.m_hWnd = NULL;
}

CTreeCtrlEx::~CTreeCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx message handlers

BOOL CTreeCtrlEx::PreTranslateMessage(MSG* pMsg) 
{
	if( !m_ToolTip.m_hWnd )
		m_ToolTip.Create(this);
	m_ToolTip.RelayEvent(pMsg);
	return CTreeCtrl::PreTranslateMessage(pMsg);
}

void CTreeCtrlEx::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	CTreeCtrl::OnDestroy();
}

void CTreeCtrlEx::OnMouseMove(UINT nFlags, CPoint point) 
{

	InvokeMethodIntIntInt(
		m_ArxControl->GetStrProperty(Prop::EventMouseMove),
		nFlags,
		point.x,
		point.y,
		m_bInvokeWithSendString);
	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CTreeCtrlEx::SetDragnDrop(BOOL bRegister)
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

void CTreeCtrlEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	HTREEITEM hItem = HitTest(point, &nFlags);
	
	/*
	HTREEITEM hOldItem = GetSelectedItem();
		
	if (hOldItem != NULL && hItem != NULL)
	{
		SetItemState(hOldItem, NULL, NULL);
		SetItemState(hOldItem, NULL, TVIS_DROPHILITED);
	}
	if (hItem != NULL)
	{
		SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
	}
	*/	
	if (m_ArxControl->GetBooleanProperty(Prop::DragnDropAllowBegin) == TRUE)
	{
		SetFocus();
		if ((hItem != NULL))// && (TVHT_ONITEM & uFlags))
		   Select(hItem, TVGN_CARET);
		BeginDragnDrop(m_ArxControl, point, m_bInvokeWithSendString);
	}
	
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CTreeCtrlEx::OnLButtonUp(UINT nFlags, CPoint point) 
{
	/*
	HTREEITEM hOldItem = GetSelectedItem();
	HTREEITEM hItem = HitTest(point, &nFlags);
		
	if (hOldItem != NULL && hItem != NULL)
	{
		SetItemState(hOldItem, NULL, NULL);
		SetItemState(hOldItem, NULL, TVIS_DROPHILITED);
	}
	if (hItem != NULL)
	{
		SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
	}
	*/	
	
	CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CTreeCtrlEx::OnRButtonUp(UINT nFlags, CPoint point) 
{

	InvokeMethod(
		m_ArxControl->GetStrProperty(Prop::EventRClick),
		m_bInvokeWithSendString);	
		
	CTreeCtrl::OnRButtonUp(nFlags, point);
}

void CTreeCtrlEx::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	*pResult = 0;

	VdclTree *pParent = (VdclTree *) GetParent();
	if (pParent->m_bDeleting)
		return;

	// get the selected item
	TV_ITEM& SelectedItem = pNMTreeView->itemNew;
	pParent->m_hOldItem = SelectedItem.hItem;
	
	//bool bFoundParent = false;
	CString sKey = pParent->Get_hItemKey(SelectedItem.hItem);

	CString sItemText = GetItemText(SelectedItem.hItem);   
		
	if (!sKey.IsEmpty()) 
	{		
		// call methods to invoke the event
		InvokeMethodStringString(
			m_ArxControl->GetStrProperty(Prop::EventSelChanged),
			sItemText,
			sKey,
			m_bInvokeWithSendString);
	}
	else if (SelectedItem.hItem != NULL)
	{		
		// call methods to invoke the event
		InvokeMethodStringLong(
			m_ArxControl->GetStrProperty(Prop::EventSelChanged),
			sItemText,
			(DWORD_PTR)SelectedItem.hItem,
			m_bInvokeWithSendString);
	}
	SelectDropTarget(SelectedItem.hItem);
	Invalidate();
}

void CTreeCtrlEx::OnRButtonDown(UINT nFlags, CPoint point)
{
	HTREEITEM hItem = HitTest(point, &nFlags);
	SelectItem(hItem);
	CTreeCtrl::OnRButtonDown(nFlags, point);
}
