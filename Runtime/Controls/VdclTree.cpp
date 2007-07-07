// VdclTree.cpp : implementation file
//

#include "stdafx.h"
#include "VdclTree.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "ArgumentsRetrieval.h"
#include "PropertyObject.h"
#include "ToolTips.h"
#include "PropertyIds.h"
#include "Workspace.h"

const int IDC_TREE = 77;
const TCHAR *pKeyStart = _T("k");


OdclTreeItem::OdclTreeItem()
{
	m_ExpandedImageIndex = -1;
	m_ImageIndex = -1;
	m_SelectedImageIndex = -1;
	hItem = NULL;

}
OdclTreeItem::~OdclTreeItem()
{
	hItem = NULL;
}
/////////////////////////////////////////////////////////////////////////////
// VdclTree

VdclTree::VdclTree()
{
	m_TreeItems.RemoveAll();
	m_bDeleting = false;
	m_hOldItem = NULL;
	m_bAutoChangeSelection = false;
}

VdclTree::~VdclTree()
{
}


BEGIN_MESSAGE_MAP(VdclTree, CStatic)
	//{{AFX_MSG_MAP(VdclTree)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_TREE, OnClick)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclk)
	ON_NOTIFY(NM_KILLFOCUS, IDC_TREE, OnKillfocus)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE, OnChildSetfocus)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnRclick)
	ON_NOTIFY(NM_RDBLCLK, IDC_TREE, OnRdblclk)
	ON_NOTIFY(NM_RETURN, IDC_TREE, OnReturn)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE, OnEndlabeledit)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE, OnDeleteitem)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE, OnItemexpanded)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE, OnItemexpanding)
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE, OnBeginlabeledit)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VdclTree message handlers

BOOL VdclTree::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL ;//|
		//TVS_DISABLEDRAGDROP; //TVS_INFOTIP

	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	if (pControl->GetBoolProperty(nShowSelectAlways))
		dwStyle = dwStyle | TVS_SHOWSELALWAYS;
	if (pControl->GetBoolProperty(nHasLines))
		dwStyle = dwStyle | TVS_HASLINES;
	if (pControl->GetBoolProperty(nLinesAtRoot))
		dwStyle = dwStyle | TVS_LINESATROOT;
	if (pControl->GetBoolProperty(nHasButtons))
		dwStyle = dwStyle | TVS_HASBUTTONS;
	if (pControl->GetBoolProperty(nEditLabels))
		dwStyle = dwStyle | TVS_EDITLABELS;
	
	if (pControl->GetBoolProperty(nSingleExpanded) == TRUE)
	{
		dwStyle = dwStyle | TVS_SINGLEEXPAND;
		m_bSingleExpand = true;
	}
	else
		m_bSingleExpand = false;

	
	RetVal = CStatic::Create(
		CString(), 
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_CLIPSIBLINGS/*|WS_CLIPCHILDREN*/,
		ArxRect,
		pParentWnd,
		nID);
	
	//VERIFY(CStatic::SubclassDlgItem(nID, pParentWnd));
	
	CRect rcChild(0,0,ArxRect.Width(), ArxRect.Height());
	m_ChildTree.Create(dwStyle, rcChild, this, IDC_TREE);
	
	//VERIFY(m_ChildTree.SubclassDlgItem(IDC_TREE, this));
	
	m_ChildTree.m_ArxControl = m_ArxControl;
	//m_ChildTree.ModifyStyle(TVS_TRACKSELECT, 0, SWP_NOSIZE | SWP_NOMOVE);
	m_ChildTree.m_ToolTip.Create(this);
	SetToolTipEx(&m_ChildTree, m_ChildTree.m_ToolTip, pControl);

	
	switch (m_ArxControl->GetLongProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}
	m_ChildTree.m_bInvokeWithSendString = m_bInvokeWithSendString;
	return RetVal;
}

int VdclTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}


void VdclTree::OnDestroy() 
{
	m_bDeleting = true;
	Clear();
	m_ChildTree.SetImageList(NULL, TVSIL_NORMAL);
	m_ChildTree.DeleteAllItems();	
	CStatic::OnDestroy();
	
}

HTREEITEM VdclTree::AddParent(CString sParentText, CString sKey, int nImage, int nSelectedImage, int nExpandedImage)
{
	
	TV_INSERTSTRUCT curTreeItem;

	curTreeItem.hParent = NULL;
	curTreeItem.item.iImage = nImage;
	curTreeItem.item.iSelectedImage = nSelectedImage;
	curTreeItem.item.pszText = sParentText.GetBuffer(sParentText.GetLength());
	curTreeItem.item.mask = TVIF_TEXT;
	
	
	HTREEITEM hItem = m_ChildTree.InsertItem(&curTreeItem);
	
	if (!sKey.IsEmpty())
	{
		OdclTreeItem *ThisItem = new OdclTreeItem;
		ThisItem->hItem = hItem;
		ThisItem->sKey = sKey;
		ThisItem->m_ExpandedImageIndex = nExpandedImage;
		ThisItem->m_ImageIndex = nImage;
		ThisItem->m_SelectedImageIndex = nSelectedImage;		
		m_TreeItems.AddTail(ThisItem);
	}
	m_ChildTree.SetItemImage(hItem, nImage, nSelectedImage);
	m_ChildTree.Invalidate();
	return hItem;
}

void VdclTree::SelectItem(CString sKey)
{
	HTREEITEM hItem = Get_hItem(sKey);
	if (hItem == NULL)
	{
		theWorkspace.DisplayAlert(_T("Node item Not Found"));
		return;
	}

	m_ChildTree.SelectItem(hItem);
}

OdclTreeItem *VdclTree::GetTreeItem(int nIndex)
{
	POSITION pos = m_TreeItems.FindIndex(nIndex);
	if (pos != NULL)
		return m_TreeItems.GetAt(pos);
	else
		return NULL;
}
//*****************************************************************************
// 
// Method: VdclTree::Get_hItem()
// 
// Purpose: [find the key and return the HTREEITEM found]
// 
// Parameters:  
//		[sKey]:  [TODO: Write the description of this parameter]
// 
// Returns:	HTREEITEM
// 
//*****************************************************************************
HTREEITEM VdclTree::Get_hItem(CString sKey)
{
	HTREEITEM hReturn = NULL;

	int nCount = m_TreeItems.GetCount();
	if (nCount == 0)
		return NULL;
	
	// do loop to find the hitem by key
	for (int i=0; i < nCount; i++)
	{
		
		if (GetTreeItem(i)->sKey == sKey)
		{
			hReturn = GetTreeItem(i)->hItem;
			// end loop
			return hReturn;
		}
	}

	return hReturn;
}

CString VdclTree::GetNextAvailableKey()
{
	HTREEITEM hReturn = NULL;

	int nCount = m_TreeItems.GetCount();
	CString sNextKey;
	char value[80];
	// do loop to find the hitem by key
	for (int i=0; i < nCount+1; i++)
	{
		_ltoa(i, value, 10);
		sNextKey = CString(pKeyStart) + value;
		if (Get_hItem(sNextKey) == NULL)
		{
			return sNextKey;		
		}
	}

	return sNextKey;
}

//*****************************************************************************
// 
// Method: VdclTree::Get_hItemKey()
// 
// Purpose: [get the the key by HTREEITEM]
// 
// Parameters:  
//		[hItem]:  [TODO: Write the description of this parameter]
// 
// Returns:	CString
// 
//*****************************************************************************
CString VdclTree::Get_hItemKey(HTREEITEM hItem)
{
	CString sReturn;
	// do loop to find hte hitem by key
	for (int i=0; i < m_TreeItems.GetCount(); i++)
	{
		OdclTreeItem *ThisItem = GetTreeItem(i);
		if (ThisItem != NULL)
		{
			if (ThisItem->hItem == hItem)
			{
				sReturn = ThisItem->sKey;
				// set i so the loop will end
				i = m_TreeItems.GetCount();		
			}
		}
	}
	return sReturn;
}

//*****************************************************************************
// 
// Method: VdclTree::FindItemIndex()
// 
// Purpose: [Find the key and return the index]
// 
// Parameters:  
//		[sKey]:  [TODO: Write the description of this parameter]
// 
// Returns:	int
// 
//*****************************************************************************
int VdclTree::FindItemIndex(CString sKey)
{
	int nReturn = -1;
	// do loop to find hte hitem by key
	for (int i=0; i < m_TreeItems.GetCount(); i++)
	{
		if (GetTreeItem(i) != NULL)
		{
			if (GetTreeItem(i)->sKey == sKey)
			{
				nReturn = i;
				// set i so the loop will end
				i = m_TreeItems.GetCount();		
			}
		}
	}
 	return nReturn;
}

HTREEITEM VdclTree::AddChild(CString sChildText, CString sParentKey, CString sKey, int nImage, int nSelectedImage, int nExpandedImage)
{
	TV_INSERTSTRUCT curTreeItem;
	
	curTreeItem.hParent = Get_hItem(sParentKey);

	if(curTreeItem.hParent == NULL)
		return false;
	
	HTREEITEM hItemOld = m_ChildTree.GetSelectedItem();
	curTreeItem.hInsertAfter = NULL;
	curTreeItem.item.iImage = nImage;
	curTreeItem.item.iSelectedImage = nSelectedImage;
	curTreeItem.item.pszText = sChildText.GetBuffer(sChildText.GetLength());
	curTreeItem.item.mask = TVIF_TEXT;
	
	HTREEITEM hItem = m_ChildTree.InsertItem(&curTreeItem);
	
	if (!sKey.IsEmpty())
	{
		OdclTreeItem *ThisItem = new OdclTreeItem;
		ThisItem->hItem = hItem;
		ThisItem->sKey = sKey;
		ThisItem->m_ExpandedImageIndex = nExpandedImage;
		ThisItem->m_ImageIndex = nImage;
		ThisItem->m_SelectedImageIndex = nSelectedImage;
		m_TreeItems.AddTail(ThisItem);
	}
	m_ChildTree.SetItemImage(hItem, nImage, nSelectedImage);
	/*
	if (m_bAutoChangeSelection)
	{
		m_ChildTree.SetItemState(hItem, NULL, NULL);
		m_ChildTree.SetItemState(hItem, NULL, TVIS_DROPHILITED);	
		m_ChildTree.SelectItem(hItem);
		m_ChildTree.SelectItem(hItemOld);
	}
	*/
	m_ChildTree.Invalidate();
	return hItem;
}


HTREEITEM VdclTree::AddChild(CString sChildText, ULONG lParent, int nImage, int nSelectedImage, int nExpandedImage)
{
	TV_INSERTSTRUCT curTreeItem;
	
	if (lParent == 0)
		return 0;
	
	HTREEITEM hItemOld = m_ChildTree.GetSelectedItem();
	
	curTreeItem.hParent = (HTREEITEM) lParent;
	curTreeItem.hInsertAfter = NULL;
	curTreeItem.item.iImage = nImage;
	curTreeItem.item.iSelectedImage = nSelectedImage;
	curTreeItem.item.pszText = sChildText.GetBuffer(sChildText.GetLength());
	curTreeItem.item.mask = TVIF_TEXT;
	
	HTREEITEM hItem = m_ChildTree.InsertItem(&curTreeItem);
	
	m_ChildTree.SetItemImage(hItem, nImage, nSelectedImage);
	/*
	if (m_bAutoChangeSelection)
	{
		m_ChildTree.SetItemState(hItem, NULL, NULL);
		m_ChildTree.SetItemState(hItem, NULL, TVIS_DROPHILITED);
		m_ChildTree.SelectItem(hItem);
		m_ChildTree.SelectItem(hItemOld);
	}
	*/
	m_ChildTree.Invalidate();
	return hItem;
}

HTREEITEM VdclTree::AddAfter(CString sChildText, ULONG lRefKey, int nImage, int nSelectedImage, int nExpandedImage)
{
	TV_INSERTSTRUCT curTreeItem;
	
	curTreeItem.hInsertAfter = (HTREEITEM)lRefKey;
	curTreeItem.hParent = m_ChildTree.GetParentItem(curTreeItem.hInsertAfter);
	curTreeItem.item.iImage = nImage;
	curTreeItem.item.iSelectedImage = nSelectedImage;
	curTreeItem.item.pszText = sChildText.GetBuffer(sChildText.GetLength());
	curTreeItem.item.mask = TVIF_TEXT;
	
	HTREEITEM hItem = m_ChildTree.InsertItem(&curTreeItem);
	
	m_ChildTree.SetItemImage(hItem, nImage, nSelectedImage);
	m_ChildTree.Invalidate();


	return hItem;
}

HTREEITEM VdclTree::AddAfter(CString sChildText, CString sRefKey, CString sKey, int nImage, int nSelectedImage, int nExpandedImage)
{
	TV_INSERTSTRUCT curTreeItem;
	
	curTreeItem.hInsertAfter = Get_hItem(sRefKey);
	curTreeItem.hParent = m_ChildTree.GetParentItem(curTreeItem.hInsertAfter);
	//curTreeItem.hParent = NULL;
	curTreeItem.item.iImage = nImage;
	curTreeItem.item.iSelectedImage = nSelectedImage;
	curTreeItem.item.pszText = sChildText.GetBuffer(sChildText.GetLength());
	curTreeItem.item.mask = TVIF_TEXT;
	
	HTREEITEM hItem = m_ChildTree.InsertItem(&curTreeItem);
	
	if (!sKey.IsEmpty())
	{
		OdclTreeItem *ThisItem = new OdclTreeItem;

		ThisItem->hItem = hItem;
		ThisItem->sKey = sKey;
		ThisItem->m_ExpandedImageIndex = nExpandedImage;
		ThisItem->m_ImageIndex = nImage;
		ThisItem->m_SelectedImageIndex = nSelectedImage;
		m_TreeItems.AddTail(ThisItem);
	}
	m_ChildTree.SetItemImage(hItem, nImage, nSelectedImage);
	m_ChildTree.Invalidate();
	return hItem;
}


//*****************************************************************************
// 
// Method: VdclTree::Clear()
// 
// Purpose: [Clear the tree control]
// 
// Parameters: none
// 
// Returns:	nothing
// 
//*****************************************************************************
void VdclTree::Clear()
{
	m_bDeleting = true;
	m_ChildTree.DeleteAllItems();

	for (int i=m_TreeItems.GetCount()-1; i>=0; i--)
	{
		POSITION pos = m_TreeItems.FindIndex(i);
		if (pos != NULL)
		{
			OdclTreeItem *pTreeItem = m_TreeItems.GetAt(pos);
			
			m_TreeItems.RemoveAt(pos);
			delete pTreeItem;		
		}
	}
	m_bDeleting = false;
}
//*****************************************************************************
// 
// Method: VdclTree::GetParentInfo()
// 
// Purpose: [Send the parent info back to AutoLISP]
// 
// Parameters:  
//		[sKey]:  [TODO: Write the description of this parameter]
// 
// Returns:	bool
// 
//*****************************************************************************
bool VdclTree::GetParentInfo(CString sKey) 
{
	//bool bFoundParent = false;
	int nIndex = FindItemIndex(sKey);

	if (nIndex == -1)
		return false;

	HTREEITEM hItem = GetTreeItem(nIndex)->hItem;
	if (hItem == NULL)
	{
		acedRetNil();
		return true;
	}
	
	HTREEITEM hParentItem = m_ChildTree.GetParentItem(hItem);

	if (hParentItem == NULL)
	{
		acedRetNil();
		return true;
	}
	
 	// do loop to get the parent's key
	CString sParentKey = Get_hItemKey(hParentItem);

	acedRetStr(sParentKey);
	
	return true;
}
bool VdclTree::GetParentInfo(HTREEITEM hItem) 
{
	if (hItem == NULL)
		return false;

	HTREEITEM hParentItem = m_ChildTree.GetParentItem(hItem);

	if (hParentItem == NULL)
	{
		acedRetNil();
		return true;
	}
	
 	// get the parent's key
	CString sParentKey = Get_hItemKey(hParentItem);

	if (sParentKey.GetLength() > 0)
	{
		acedRetStr(sParentKey);		
		return true;
	}
	
	acedRetLong((long)hParentItem);
	
	return true;
}

void VdclTree::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nEventClicked),
		m_bInvokeWithSendString);

	*pResult = 0;
}

void VdclTree::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nEventDblClicked),
		m_bInvokeWithSendString);

	*pResult = 0;
}

void VdclTree::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nEventKillFocus),
		m_bInvokeWithSendString);

	*pResult = 0;
}

void VdclTree::OnChildSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nEventSetFocus),
		m_bInvokeWithSendString);

	*pResult = 0;
}

void VdclTree::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nEventRClick),
		m_bInvokeWithSendString);
	
	*pResult = 0;
}

void VdclTree::OnRdblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nEventRDblClick),
		m_bInvokeWithSendString);

	
	*pResult = 0;
}

void VdclTree::OnReturn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// call methods to invoke the event
	InvokeMethod(
		m_ArxControl->GetStrProperty(nEventReturn),
		m_bInvokeWithSendString);

	
	*pResult = 0;
}

void VdclTree::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	
	TV_ITEM SelectedItem = pTVDispInfo->item;
	CString sKey = Get_hItemKey(SelectedItem.hItem);
	
	if (sKey != CString())
	{
		// call methods to invoke the event
		InvokeMethodString(		
			m_ArxControl->GetStrProperty(nEventBeginLabelEdit),
			sKey,
			m_bInvokeWithSendString);
	}
	else if (SelectedItem.hItem != NULL)
	{
		// call methods to invoke the event
		InvokeMethodLong(		
			m_ArxControl->GetStrProperty(nEventBeginLabelEdit),
			(long)SelectedItem.hItem,
			m_bInvokeWithSendString);
	}
	
	*pResult = 0;
}

void VdclTree::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	
	
	TV_ITEM SelectedItem = pTVDispInfo->item;
	
	CString sKey = Get_hItemKey(SelectedItem.hItem);
	CEdit* pEdit = m_ChildTree.GetEditControl();
	CString sNewText;
	pEdit->GetWindowText(sNewText);
	m_ChildTree.SetItemText(SelectedItem.hItem, sNewText);

	if (sKey != CString())
	{
		InvokeMethodStringString(
			m_ArxControl->GetStrProperty(nEventEndLabelEdit),
			sNewText,
			sKey,
			m_bInvokeWithSendString);
	}
	else if (SelectedItem.hItem != NULL)
	{
		InvokeMethodStringLong(
			m_ArxControl->GetStrProperty(nEventEndLabelEdit),
			sNewText,
			(long)SelectedItem.hItem,
			m_bInvokeWithSendString);
	}
	
	*pResult = 0;
}

void VdclTree::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	// get the selected item
	TV_ITEM SelectedItem = pNMTreeView->itemOld;
	
	assert((SelectedItem.mask & TVIF_HANDLE) == TVIF_HANDLE);
	//bool bFoundParent = false;
	CString sKey = Get_hItemKey(SelectedItem.hItem);

	CString sItemText = m_ChildTree.GetItemText(SelectedItem.hItem);   
		
	if (sKey.GetLength() > 0)
	{
		// call methods to invoke the event
		InvokeMethodStringString(
			m_ArxControl->GetStrProperty(nEventDeleteItem),
			sItemText,
			sKey,
			m_bInvokeWithSendString);
	}
	else if (SelectedItem.hItem != NULL)
	{
		// call methods to invoke the event
		InvokeMethodStringLong(
			m_ArxControl->GetStrProperty(nEventDeleteItem),
			sItemText,
			(long)SelectedItem.hItem,
			m_bInvokeWithSendString);
	}
	
	*pResult = 0;
}

void VdclTree::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	HTREEITEM hOldItem = m_ChildTree.GetSelectedItem();
	
	// get the selected item
	TV_ITEM SelectedItem = pNMTreeView->itemNew;
	UINT action = pNMTreeView->action; 
	
	CString sKey = Get_hItemKey(SelectedItem.hItem);
	
	
	
	*pResult = 0;
	
	if (m_bSingleExpand)
	{
		HTREEITEM hItem = SelectedItem.hItem;

		// Collapse the siblings after the current selection
		while (hItem != NULL)
		{
			hItem = m_ChildTree.GetNextSiblingItem(hItem);
			m_ChildTree.Expand(hItem, TVE_COLLAPSE);	
	
			OdclTreeItem *pItem = GetTreeItem(FindItemIndex(Get_hItemKey(hItem)));
	
			if (pItem != NULL)
			{
				// lets make sure the image is updated correctly. If we don't it will show the previous image.
				m_ChildTree.SetItemImage(hItem, pItem->m_ImageIndex, pItem->m_SelectedImageIndex);	
			}
			
		}

		hItem = SelectedItem.hItem;

		// Collapse the siblings before the current selection
		while (hItem != NULL)
		{
			hItem = m_ChildTree.GetPrevSiblingItem(hItem);
			m_ChildTree.Expand(hItem, TVE_COLLAPSE);	
	
			OdclTreeItem *pItem = GetTreeItem(FindItemIndex(Get_hItemKey(hItem)));
	
			if (pItem != NULL)
			{
				// lets make sure the image is updated correctly. If we don't it will show the previous image.
				m_ChildTree.SetItemImage(hItem, pItem->m_ImageIndex, pItem->m_SelectedImageIndex);	
			}
			
		}
		
	}


	HTREEITEM hItem = SelectedItem.hItem;
		
	if (m_ArxControl->GetBoolProperty(nSingleExpanded) == TRUE)	
		m_ChildTree.ModifyStyle(TVS_SINGLEEXPAND, NULL, SWP_FRAMECHANGED);
/*
	if (m_bAutoChangeSelection)
	{
		if (hOldItem != NULL && hItem != NULL)
		{
			m_ChildTree.SetItemState(hOldItem, NULL, NULL);
			m_ChildTree.SetItemState(hOldItem, NULL, TVIS_DROPHILITED);
		}
		
		if (hItem != NULL)
		{
			m_ChildTree.SelectItem(hItem);
			m_ChildTree.SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);		
		}

		if (m_ArxControl->GetBoolProperty(nSingleExpanded) == TRUE)	
			m_ChildTree.ModifyStyle(NULL, TVS_SINGLEEXPAND, SWP_FRAMECHANGED);
	}
*/
	CString sItemText = m_ChildTree.GetItemText(SelectedItem.hItem);   
	
	if (!sKey.IsEmpty())
	{	
		// call methods to invoke the event
		InvokeMethodStringString(
			m_ArxControl->GetStrProperty(nEventItemExpanded),
			sItemText,
			sKey,
			m_bInvokeWithSendString);
	}
	else		
	{	
		// call methods to invoke the event
		InvokeMethodStringLong(
			m_ArxControl->GetStrProperty(nEventItemExpanded),
			sItemText,
			(long)SelectedItem.hItem,
			m_bInvokeWithSendString);
	}
}

void VdclTree::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	HTREEITEM hOldItem = m_ChildTree.GetSelectedItem();
	
	// get the selected item
	TV_ITEM SelectedItem = pNMTreeView->itemNew;
	
	//bool bFoundParent = false;
	CString sKey = Get_hItemKey(SelectedItem.hItem);

	// do loop to find the hitem by key
	for (int i=0; i < m_TreeItems.GetCount(); i++)
	{
		if (GetTreeItem(i) != NULL)
		{
			if (GetTreeItem(i)->hItem == SelectedItem.hItem)
			{
				if (GetTreeItem(i)->m_ExpandedImageIndex > -1)
				{
					if (pNMTreeView->action == 2)
					{
						int nEIndex = GetTreeItem(i)->m_ExpandedImageIndex;
						m_ChildTree.SetItemImage(
							SelectedItem.hItem,
							GetTreeItem(i)->m_ExpandedImageIndex,
							GetTreeItem(i)->m_ExpandedImageIndex);				
					}
					else
					{
						m_ChildTree.SetItemImage(
							SelectedItem.hItem,
							GetTreeItem(i)->m_ImageIndex,
							GetTreeItem(i)->m_SelectedImageIndex);				
					}
				}
				// set i so the loop will end
				i = m_TreeItems.GetCount();		
			}
		}
	}
	
	*pResult = 0;
	
	HTREEITEM hItem = SelectedItem.hItem;
	
	if (m_ArxControl->GetBoolProperty(nSingleExpanded) == TRUE)	
		m_ChildTree.ModifyStyle(TVS_SINGLEEXPAND, NULL, SWP_FRAMECHANGED);

	/*
	if (m_bAutoChangeSelection)
	{
		if (hOldItem != NULL && hItem != NULL)
		{
			m_ChildTree.SetItemState(hOldItem, NULL, NULL);
			m_ChildTree.SetItemState(hOldItem, NULL, TVIS_DROPHILITED);
		}
		if (hItem != NULL)
		{
			m_ChildTree.SelectItem(hItem);
			m_ChildTree.SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);		
		}

		if (m_ArxControl->GetBoolProperty(nSingleExpanded) == TRUE)	
			m_ChildTree.ModifyStyle(NULL, TVS_SINGLEEXPAND, SWP_FRAMECHANGED);
	}
*/
	CString sItemText = m_ChildTree.GetItemText(SelectedItem.hItem);   

	if (!sKey.IsEmpty())
	{	
		// call methods to invoke the event
		InvokeMethodStringString(
			m_ArxControl->GetStrProperty(nEventItemExpanding),
			sItemText,
			sKey,
			m_bInvokeWithSendString);
	}
	else		
	{	
		// call methods to invoke the event
		InvokeMethodStringLong(
			m_ArxControl->GetStrProperty(nEventItemExpanding),
			sItemText,
			(long)SelectedItem.hItem,
			m_bInvokeWithSendString);
	}

	
}


void VdclTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	char sChar = nChar;
	InvokeMethodStringIntInt(m_ArxControl->GetStrProperty(nEventKeyDown), sChar, nRepCnt, nFlags, m_bInvokeWithSendString);
	
	CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
}


void VdclTree::OnSetFocus(CWnd* pOldWnd) 
{
	CStatic::OnSetFocus(pOldWnd);
	
	m_ChildTree.SetFocus();
	
}

void VdclTree::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	CRect rcChild(0,0,cx, cy);

	m_ChildTree.MoveWindow(rcChild, TRUE);
	
}

void VdclTree::SetDragnDrop(BOOL bRegister)
{
	m_ChildTree.SetDragnDrop(bRegister);
}