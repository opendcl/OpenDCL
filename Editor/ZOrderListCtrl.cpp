// ZOrderListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "objectdcl.h"
#include "ZOrderListCtrl.h"
#include "ControlHolder.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "PropertyIds.h"

#define nOnEditCopy		3
#define nOnEditPaste	22
#define nOnEditCut		24
#define nOnEditUndo		26


static CDclControlObject* FindArxControlObject(CDclFormObject *pDclForm, CString sControlName, CDclControlObject *pCtrl = NULL )
{
	CDclControlObject *pRetObject = NULL;
		
	if (pDclForm == NULL)
		return NULL;

	// create a position variable to hold the converted ArxControlIndex
	POSITION pos = pDclForm->GetControlList().GetHeadPosition();
	while (pos)
	{
		CDclControlObject *pControl = pDclForm->GetControlList().GetNext(pos);
		if (pControl->GetStrProperty(nName) == sControlName && pCtrl != pControl)
			return pControl;
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CZOrderListCtrl

CZOrderListCtrl::CZOrderListCtrl()
{
	m_pView = NULL;
}

CZOrderListCtrl::~CZOrderListCtrl()
{
}


BEGIN_MESSAGE_MAP(CZOrderListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CZOrderListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZOrderListCtrl message handlers

void CZOrderListCtrl::ClearList(CObjectDCLView *pView)
{
	m_pView = pView;
	// delete all the items
	if (m_hWnd)
		DeleteAllItems();
}
void CZOrderListCtrl::AddControlToList(CString sName, int nType)
{
	
	//int nIndex = InsertItem(GetItemCount(), sName, nType-2);

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_INDENT;
	lvItem.iItem = GetItemCount();
	lvItem.iSubItem = 0;
	lvItem.pszText = sName.LockBuffer();
	lvItem.cchTextMax = sName.GetLength() + 1;
	if (nType == nType100)
		lvItem.iImage = m_ImageList.GetImageCount() -1;
	else
		lvItem.iImage = nType - 2;
	lvItem.iIndent = 0;
	
	// insert the row item
	int nIndex = InsertItem(&lvItem);
	assert(nIndex > -1);
	SetItemData(nIndex, nType);
}

void CZOrderListCtrl::RemoveControlFromList(CString sName)
{
	for (int i=0; i<GetItemCount(); i++)
	{		
		if (GetItemText(i, 0) == sName)
		{
			DeleteItem(i);
			return;
		}
	}
}
void CZOrderListCtrl::RenameControlInList(CString sOldName, CString sNewName)
{
	for (int i=0; i<GetItemCount(); i++)
	{	
		if (GetItemText(i, 0) == sOldName)
		{
			SetItemText(i, 0, sNewName);
			return;
		}
	}
}

void CZOrderListCtrl::SelectItem(CString sName, bool bEnsureVisible)
{
	for (int i=0; i<GetItemCount(); i++)
	{
		if (GetItemText(i, 0) == sName)
		{
			SetItem(i, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
			EnsureVisible(i, bEnsureVisible);			
			return;
		}
	}
}

void CZOrderListCtrl::ClearSelection()
{
	for (int i=0; i<GetItemCount(); i++)
	{
		SetItemState(i, NULL, LVIS_SELECTED);
	}
}

int CZOrderListCtrl::GetSelectedCount() 
{
	// we need to count of the selected items
	int nSelCount=0;
	for (int i=0;i<GetItemCount(); i++)
	{
		UINT nState = GetItemState(i, LVIS_SELECTED);
		if (nState == LVIS_SELECTED)
			nSelCount++;
	}
	return nSelCount;
}

void CZOrderListCtrl::BringtofrontBy1() 
{
	// we need to count of the selected items
	int nSelCount=GetSelectedCount();
	
	int nNumberSelected = 0;
	int nNumberUnSelected = 0;

	// here we need to exit if we find more than one selected and they are all at the end
	int i;
  for (i=GetItemCount()-1; i>=0; i--)
	{
		UINT nState = GetItemState(i, LVIS_SELECTED);
		if (nState == LVIS_SELECTED)
		{
			nNumberSelected++;
		}
		else
		{
			nNumberUnSelected++;
		}

		if (nNumberSelected == nSelCount && nNumberUnSelected ==0) 
			return;
	}

	// lets do the loop to move the control(s) forward by one
	for (i=GetItemCount()-1; i>=0; i--)
	{
		UINT nState = GetItemState(i, LVIS_SELECTED);
		if (nState == LVIS_SELECTED)
		{
			// get the item info
			int nType = GetItemData(i);
			CString sName = GetItemText(i, 0);
			
			// add a items into their new positions
			int nIndex = InsertItem(i+2, sName, nType-2);
			SetItemData(nIndex, nType);
			SetItemState(nIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
			
			DeleteItem(i);			
		}
	}

	// make the call to finish the reordering by actually moving the controls in the current CView
	DoZOrderUpdate();
}


void CZOrderListCtrl::SentToBackBy1() 
{
	// we need to count of the selected items
	int nSelCount=GetSelectedCount();
	
	int nNumberSelected = 0;
	int nNumberUnSelected = 0;

	// here we need to exit if we find more than one selected and they are all at the end
  int i;
	for (i=0; i<GetItemCount(); i++)
	{
		UINT nState = GetItemState(i, LVIS_SELECTED);
		if (nState == LVIS_SELECTED)
		{
			nNumberSelected++;
		}
		else
		{
			nNumberUnSelected++;
		}

		if (nNumberSelected == nSelCount && nNumberUnSelected ==0) 
			return;
	}

	// lets do the loop to move the control(s) back by one
	for (i=0; i<GetItemCount(); i++)
	{
		UINT nState = GetItemState(i, LVIS_SELECTED);
		if (nState == LVIS_SELECTED)
		{
			// get the item info
			int nType = GetItemData(i);
			CString sName = GetItemText(i, 0);
			
			int nNewIndex = i-1;
			if (nNewIndex < 0)
			{
				nNewIndex = 0;
				return;
			}
			
			DeleteItem(i);

			// add a items into their new positions
			int nIndex = InsertItem(nNewIndex, sName, nType-2);
			SetItemData(nIndex, nType);
			SetItemState(nIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
			
		}
	}

	// make the call to finish the reordering by actually moving the controls in the current CView
	DoZOrderUpdate();


}

void CZOrderListCtrl::Bringtofront() 
{
	// we need to count of the selected items
	int nSelCount=GetSelectedCount();
			
	int nNumberSelected = 0;
	int nNumberUnSelected = 0;

	// here we need to exit if we find more than one selected and they are all at the end
  int i;
	for (i=GetItemCount()-1; i>=0; i--)
	{
		UINT nState = GetItemState(i, LVIS_SELECTED);
		if (nState == LVIS_SELECTED)
		{
			nNumberSelected++;
		}
		else
		{
			nNumberUnSelected++;
		}

		if (nNumberSelected == nSelCount && nNumberUnSelected == 0) 
			return;
	}

	// we need to create 2 arrays to hold the info.
	CString *pNames = new CString[nSelCount];
	int *pTypes = new int[nSelCount];
	
	// reset the selection count
	nSelCount=0;

	// lets extract the selected items
	for (i=0; i<GetItemCount(); i++)
	{
		UINT nState = GetItemState(i, LVIS_SELECTED);
		if (nState == LVIS_SELECTED)
		{
			// get the item info
			pTypes[nSelCount] = GetItemData(i);
			pNames[nSelCount] = GetItemText(i, 0);
			
			nSelCount++;
		}
	}

	// lets delete the selected items
	for (i=GetItemCount()-1; i>=0; i--)
	{
		UINT nState = GetItemState(i, LVIS_SELECTED);
		if (nState == LVIS_SELECTED)
		{
			DeleteItem(i);
		}
	}

	// lets insert the items in their new positions
	for (i=0; i<nSelCount; i++)
	{
		// add a items into their new positions
		int nIndex = InsertItem(GetItemCount(), pNames[i], pTypes[i]-2);
		SetItemData(nIndex, pTypes[i]);
		SetItemState(nIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);					
	}

	// we need to delete the items now
	delete [] pNames;
	delete [] pTypes;

	// make the call to finish the reordering by actually moving the controls in the current CView
	DoZOrderUpdate();
}



void CZOrderListCtrl::SendtoBack() 
{
	// we need to count of the selected items
	int nSelCount=GetSelectedCount();
		
	int nNumberSelected = 0;
	int nNumberUnSelected = 0;

	// here we need to exit if we find more than one selected and they are all at the end
  int i;
	for (i=0; i<GetItemCount(); i++)
	{
		UINT nState = GetItemState(i, LVIS_SELECTED);
		if (nState == LVIS_SELECTED)
		{
			nNumberSelected++;
		}
		else
		{
			nNumberUnSelected++;
		}

		if (nNumberSelected == nSelCount && nNumberUnSelected == 0) 
			return;
	}

	// we need to create 2 arrays to hold the info.
	CString *pNames = new CString[nSelCount];
	int *pTypes = new int[nSelCount];
	
	// reset the selection count
	nSelCount=0;

	// lets extract the selected items
	for (i=0; i<GetItemCount(); i++)
	{
		UINT nState = GetItemState(i, LVIS_SELECTED);
		if (nState == LVIS_SELECTED)
		{
			// get the item info
			pTypes[nSelCount] = GetItemData(i);
			pNames[nSelCount] = GetItemText(i, 0);
			
			nSelCount++;
		}
	}

	// lets delete the selected items
	for (i=GetItemCount()-1; i>=0; i--)
	{
		UINT nState = GetItemState(i, LVIS_SELECTED);
		if (nState == LVIS_SELECTED)
		{
			DeleteItem(i);
		}
	}

	// lets insert the items in their new positions
	for (i=nSelCount-1; i>=0; i--)
	{
		// add a items into their new positions
		int nIndex = InsertItem(0, pNames[i], pTypes[i]-2);
		SetItemData(nIndex, pTypes[i]);
		SetItemState(nIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}

	// we need to delete the items now
	delete [] pNames;
	delete [] pTypes;

	// make the call to finish the reordering by actually moving the controls in the current CView
	DoZOrderUpdate();
}


void CZOrderListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* plvdi = (LV_DISPINFO*)pNMHDR;
	
	if (plvdi->item.mask < UINT(GetItemCount()) && 
		plvdi->item.mask >= 0)
	{
		// get the item'sname
		CString sName = GetItemText(plvdi->item.mask, 0);
		// get the numver of selected items
		int nSelCount=GetSelectedCount();
		if (m_pView != NULL)
		{
			// if only one is selected
			if (nSelCount == 1)
			{
				// make the calls to the CView to select the control
				m_pView->m_SelectedControl.m_pControl = NULL;
				m_pView->m_SelectedControl.m_pControl = NULL;
				m_pView->ClearSelection();
				m_pView->SelectControl(sName);
			}
			else
			{
				// make the calls to the CView to select all the controls
				m_pView->m_SelectedControl.m_pControl = NULL;
				m_pView->m_SelectedControl.m_pControl = NULL;
				m_pView->ClearSelection(false);

				// loop to select all the controls
				for (int i=0;i<GetItemCount(); i++)
				{
					// here we are adding the controls to the selection list.
					UINT nState = GetItemState(i, LVIS_SELECTED);
					if (nState == LVIS_SELECTED)
						m_pView->SelectControl(GetItemText(i, 0));
				}				
			}
		}
	}
	*pResult = 0;
}


// this method will reorder the entire list of controls in the current CView
void CZOrderListCtrl::DoZOrderUpdate()
{
	// this temp list will be used to re-order the controls in the dcl form's control list.
	CList< CDclControlObject* > ControlList;
	CDclControlObject *pDclFormPropHolder;
	CDclControlObject *pCtrlObject;
	POSITION pos = NULL;
	
	// first we need to get the control object that stores the properties for hte dcl form.
	pDclFormPropHolder = m_pView->m_pThisDclForm->GetControlProperties();

	
	// here we need to exit if we find more than one selected and they are all at the end
  int i;
	for (i=0; i<GetItemCount(); i++)
	{
		// get the item name so we can extract it.
		CString sName = GetItemText(i, 0);

		// call the method that extracts the control object by it's name from the list
		pCtrlObject = FindArxControlObject(m_pView->m_pThisDclForm, sName);

		// add it to the list.
		ControlList.AddTail(pCtrlObject);
	}

	// here we need to loop thru the dcl form control list to extract all the deleted controls so they don't get lost in space
	for (i=0; i<ControlList.GetCount(); i++)
	{
		pos = m_pView->m_pThisDclForm->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			// get the control
			pCtrlObject = m_pView->m_pThisDclForm->GetControlList().GetAt(pos);
			
			// if the control has been marked as deleted we need to place it in the front of the list
			if (pCtrlObject->m_Delete == TRUE)
				// add it to the list.
				ControlList.AddHead(pCtrlObject);	
		}
	}

	pos = ControlList.GetHeadPosition();
	while( pos )
	{
		CDclControlObject* pDclControl = ControlList.GetNext( pos );
		m_pView->m_pThisDclForm->ReorderControl( pDclControl, false, true );
		CControlHolder* pControlWnd = (CControlHolder*)m_pView->GetDlgItem(pDclControl->m_Id);
		assert( pControlWnd != NULL );
		if( pControlWnd )
			// move this control to the front of the Zorder. This is ok because we do it to every control to get the correct zorder position set.
			pControlWnd->SetWindowPos( &CWnd::wndTop, 0, 0, nNotSet, nNotSet, SWP_NOSIZE | SWP_NOMOVE );
	}
	m_pView->m_pThisDclForm->ReindexControls();
	m_pView->MoveGripRectsForward();
	m_pView->SetFocus();
}

BOOL CZOrderListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN ||  pMsg->message == WM_CHAR )
	{		
		switch (pMsg->wParam)
		{
			case VK_DELETE:
				{
					m_pView->DeleteSelectedControls();
					return CListCtrl::PreTranslateMessage(pMsg);
				break;
				}
			case nOnEditCopy:
				if (m_pView != NULL)
 					m_pView->OnEditCopy();
				break;
			case nOnEditPaste:
				if (m_pView != NULL)
 					m_pView->OnEditPaste();
				break;
			case nOnEditCut:
				if (m_pView != NULL)
 					m_pView->OnEditCut();
				break;
			case nOnEditUndo:
				if (m_pView != NULL)
 					m_pView->OnEditUndo();
				break;
			default:
				if (m_pView != NULL)
 					m_pView->PreTranslateMessage(pMsg);
				break;
			
		}	
		/*char value[80];
		_ltoa(pMsg->wParam, value, 10);
		AfxGetApp()->m_pMainWnd->SetWindowText(value);
		*/		
	}	
	return CListCtrl::PreTranslateMessage(pMsg);
}

void CZOrderListCtrl::OnEditPaste() 
{
	if (m_pView != NULL)
 		m_pView->OnEditPaste();
	
	
}
