// ZOrderListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Editor.h"
#include "ZOrderListCtrl.h"
#include "ControlHolder.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "PropertyIds.h"

#define nOnEditCopy		3
#define nOnEditPaste	22
#define nOnEditCut		24
#define nOnEditUndo		26


static TDclControlPtr FindArxControlObject(TDclFormPtr pDclForm, CString sControlName, TDclControlPtr pCtrl = NULL )
{
	if (pDclForm == NULL)
		return NULL;

	const TDclControlList& Controls = pDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		if (pDclControl->GetStringProperty(Prop::Name) == sControlName && pCtrl != pDclControl)
			return pDclControl;
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
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CZOrderListCtrl::OnCustomdraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZOrderListCtrl message handlers

void CZOrderListCtrl::ClearList(COpenDCLView *pView)
{
	m_pView = pView;
	if( m_hWnd )
		DeleteAllItems();
}

void CZOrderListCtrl::AddControlToList( TDclControlPtr pDclControl,
																				bool bSelected /*= true*/,
																				bool bEnsureVisible /*= false*/ )
{
	CString sName = pDclControl->GetStringProperty( Prop::Name );
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT;
	lvItem.iItem = GetItemCount();
	lvItem.iSubItem = 0;
	lvItem.pszText = sName.LockBuffer();
	lvItem.cchTextMax = sName.GetLength() + 1;
	ControlType nType = pDclControl->GetType();
	if (nType == CtlFileDlgCtrl)
		lvItem.iImage = m_ImageList.GetImageCount() - 1;
	else
		lvItem.iImage = nType - 2;
	lvItem.iIndent = 0;

	if( bSelected )
	{
		lvItem.mask |= LVIF_STATE;
		lvItem.state = LVIS_SELECTED;
		lvItem.stateMask = LVIS_SELECTED;
	}

	// insert the row item
	int nIndex = InsertItem(&lvItem);
	assert(nIndex > -1);
	SetItemData(nIndex, nType);
	if( bEnsureVisible )
		EnsureVisible( nIndex, FALSE );
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
			SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			if( bEnsureVisible )
				EnsureVisible(i, FALSE);
			Invalidate();
			return;
		}
	}
}

void CZOrderListCtrl::ClearSelection()
{
	for (int i=0; i<GetItemCount(); i++)
		SetItemState(i, 0, LVIS_SELECTED);
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
		if (m_pView != NULL)
		{
			m_pView->ClearSelection(false);
			// loop to select all the controls
			for (int i=0;i<GetItemCount(); i++)
			{
				if (GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
					m_pView->SelectControl(GetItemText(i, 0));
			}				
		}
	}
	*pResult = 1;
}


// this method will reorder the entire list of controls in the current CView
void CZOrderListCtrl::DoZOrderUpdate()
{
	// this temp list will be used to re-order the controls in the dcl form's control list.
	std::list< TDclControlPtr > ControlList;

	for( int idx = 0; idx < GetItemCount(); ++idx )
	{
		TDclControlPtr pDclControl = FindArxControlObject( m_pView->m_pThisDclForm, GetItemText( idx, 0 ) );
		if( pDclControl )
			ControlList.push_back( pDclControl );
	}

	// here we need to loop thru the dcl form control list to extract all the deleted controls so they don't get lost in space
	const TDclControlList& Controls = m_pView->m_pThisDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		if (pDclControl->IsDeleted())
			ControlList.push_front(pDclControl);	
	}

	for( TDclControlList::const_iterator iter = ControlList.begin(); iter != ControlList.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		m_pView->m_pThisDclForm->ReorderControl( pDclControl, false, true );
		CWnd* pControlHolder = pDclControl->m_pCtrlHolder;
		assert( pControlHolder != NULL );
		if( pControlHolder )
			pControlHolder->SetWindowPos( &CWnd::wndTop, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE );
	}
	m_pView->m_pThisDclForm->ReindexControls();
	m_pView->MoveGripsToTop();
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
	}	
	return CListCtrl::PreTranslateMessage(pMsg);
}

void CZOrderListCtrl::OnEditPaste() 
{
	if (m_pView != NULL)
 		m_pView->OnEditPaste();
}

void CZOrderListCtrl::OnCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	*pResult = CDRF_DODEFAULT;
	if( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
  {
    *pResult = CDRF_NOTIFYITEMDRAW;
  }
  else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
  {
		if( GetItemState( pLVCD->nmcd.dwItemSpec, LVIS_SELECTED ) )
			pLVCD->clrTextBk = RGB(192,192,192);
  }
}
