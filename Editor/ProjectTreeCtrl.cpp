// ProjectTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ProjectTreeCtrl.h"
#include "Project.h"
#include "ObjectDCL.h"
#include "ObjectDCLView.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "ChildFrm.h"
#include "PurchaseMode.h"
#include "ControlTypes.h"
#include "PropertyIds.h"
#include "SharedRes.h"

#define nNotSet -1


static CString StripPathFromFileName(CString sFullPath)
{
	CString sShortName = sFullPath;
	sShortName.MakeReverse();
	sShortName = sShortName.SpanExcluding(_T("\\/:"));
	sShortName.MakeReverse();
	return sShortName;
}


static CString FindTabCaption2(CDclFormObject *pDclTab, int nTabIndex)
{
	CDclControlObject* pControl = pDclTab->FindFirstControlOfType(CtlTabStrip);
	if (pControl)
		return pControl->GetPropertyListItem(nTabsCaption, nTabIndex);
	return CString();
}


static CString FindTabCaption(CDclFormObject *pDclTab)
{
	// do loop to add all the tree items
	for (int i=0; i<activeProject->GetDclFormList().GetCount(); i++)
	{
		POSITION pos = activeProject->GetDclFormList().FindIndex(i);
		if (pos != NULL)
		{
			CDclFormObject *pDcl = activeProject->GetDclFormList().GetAt(pos);
			if (pDcl != NULL)
			{
				if (pDclTab->m_ParentName == pDcl->m_UniqueName)
				{					
					return FindTabCaption2(pDcl, pDclTab->m_TabIndex);
				}
			}			
		}
	}
	return CString();
}


/////////////////////////////////////////////////////////////////////////////
// CProjectTreeCtrl

CProjectTreeCtrl::CProjectTreeCtrl()
: mpProject( NULL )
, mpDocument( NULL )
, mhtiModalParent( NULL )
, mhtiModelessParent( NULL )
, mhtiDockableParent( NULL )
, mhtiConfigParent( NULL )
, mhtiFileDialogParent( NULL )
, mhtiAutoLispFileParent( NULL )
, mhtiAutoLispFile( NULL )
, mhtiOdsFileParent( NULL )
, mhtiOdsFile( NULL )
, mhtiAxFilesParent( NULL )
{
}

CProjectTreeCtrl::~CProjectTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CProjectTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CProjectTreeCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectTreeCtrl message handlers

void CProjectTreeCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	
	HTREEITEM hItem = GetSelectedItem();
	if (hItem == NULL)
		return;// exit out if the user did not double click on a item
	
	if (mhtiAutoLispFile == hItem)
	{
		// update the project tree's autolisp file name
		SetItemText(mhtiAutoLispFile, activeProject->QueryForLispFileName());
		return;
	}
	
	if (mhtiOdsFile == hItem)
	{
		// update the project tree's ODS file name
		SetItemText(mhtiOdsFile, activeProject->QueryForOdsFileName());
		// and save the file
		activeProject->SaveDistFile();
		return;
	}

	for (int i=0; i<mpProject->GetDclFormList().GetCount(); i++)
	{
		POSITION pos = mpProject->GetDclFormList().FindIndex(i);
		if (pos != NULL)
		{
			CDclFormObject *pDcl = mpProject->GetDclFormList().GetAt(pos);
			if (pDcl->m_htiTreeItem == hItem)
			{
				if (pDcl->m_pMdiChildWnd == NULL)
					((CObjectDCLApp*)AfxGetApp())->OpenExistingForm(pDcl);
				else
					pDcl->m_pMdiChildWnd->SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
				return;
			}
		}
	}
}

void CProjectTreeCtrl::RemoveViewPointer(CView *pView)
{
	if(!mpProject)
		return;
	for (int i=0; i<mpProject->GetDclFormList().GetCount(); i++)
	{
		POSITION pos = mpProject->GetDclFormList().FindIndex(i);
		if (pos != NULL)
		{
			CDclFormObject *pDcl = mpProject->GetDclFormList().GetAt(pos);
			if (pDcl->m_pMdiChildWnd == (CChildFrame*)pView->GetParentFrame())
			{
				pDcl->m_pMdiChildWnd = NULL;
			}
		}
	}
}

void CProjectTreeCtrl::DeleteChildTab(HTREEITEM hChild)
{
	if (hChild == NULL)
		return;
	
	try
	{
		for (int i=0; i<mpProject->GetDclFormList().GetCount(); i++)
		{
			POSITION pos = mpProject->GetDclFormList().FindIndex(i);
			if (pos != NULL)
			{
				CDclFormObject *pDcl = mpProject->GetDclFormList().GetAt(pos);
				if (pDcl->m_htiTreeItem == hChild)
				{
					pDcl->m_bDeleted = true;
				}
			}
		}
		DeleteItem(hChild);
	}
	catch(...)
	{
	}
}


void CProjectTreeCtrl::SetupProjectTree(CProject* pProject /*= NULL*/)
{
	ClearTree();
	mpProject = pProject;

	if(mpProject)
	{
		// do loop to add all the tree items
		for (int i=0; i<mpProject->GetDclFormList().GetCount(); i++)
		{
			POSITION pos = mpProject->GetDclFormList().FindIndex(i);
			if (pos != NULL)
			{
				CDclFormObject *pDcl = mpProject->GetDclFormList().GetAt(pos);
				if (pDcl != NULL)
					AddFormToTree(pDcl, false);
			}
		}
	}
	
	CString sText;
	// set the lisp file name
	sText = theWorkspace.LoadResourceString(IDS_LSPFILENAME);
	mhtiAutoLispFileParent = InsertItem(sText);
	SetItemImage(mhtiAutoLispFileParent, 1,1);

	if (!mpProject || mpProject->m_LispFileName.IsEmpty())
		sText = theWorkspace.LoadResourceString(IDS_NONE);
	else
		sText = mpProject->m_LispFileName;
	// add the autolisp file name itself
	mhtiAutoLispFile = InsertItem(sText, mhtiAutoLispFileParent);
	SetItemImage(mhtiAutoLispFile, 6,6);	

	// set the distribution file name
	sText = theWorkspace.LoadResourceString(IDS_DISTFILE);
	mhtiOdsFileParent = InsertItem(sText);
	SetItemImage(mhtiOdsFileParent, 1,1);

	if (!mpProject || mpProject->m_DistFileName.IsEmpty())
		sText = theWorkspace.LoadResourceString(IDS_NONE);
	else
		sText = mpProject->m_DistFileName;
	// add the distribution file name itself
	mhtiOdsFile = InsertItem(sText, mhtiOdsFileParent);
	SetItemImage(mhtiOdsFile, 7,7);
}

void CProjectTreeCtrl::ClearTree()
{
	// remove all the items
	DeleteAllItems();
	// set all the htree items to null
	mhtiAutoLispFileParent = NULL;
	mhtiOdsFileParent = NULL;
	mhtiModalParent = NULL;
	mhtiModelessParent = NULL;
	mhtiDockableParent = NULL;
	mhtiConfigParent = NULL;
	mhtiAutoLispFile = NULL;
	mhtiOdsFile = NULL;
	mhtiAxFilesParent = NULL;
	mhtiFileDialogParent = NULL;
	mpProject = NULL;
}

void CProjectTreeCtrl::AddActiveXFileTree(CString sFileName)
{
	bool bAlreadyLoaded = false;

	if (sFileName.GetLength() == 0)
		return;

	sFileName = StripPathFromFileName(sFileName);

	
	// do a loop to ensure we don't add files more than once.
	for (int i=0; i<activeProject->m_ActiveXFiles.GetSize(); i++)
	{
		// if the file is already in the list, set the flag not to add it again.
		if (activeProject->m_ActiveXFiles[i] == sFileName)
			bAlreadyLoaded = true;
	}
	// if the active file has not been added already.
	if (!bAlreadyLoaded)
		// then add it.
		activeProject->m_ActiveXFiles.Add(sFileName);
	else
		// if the file has already been loaded we need to exit here so it won't show up twice in the tree control
		return;

	// add the parent tree item
	if (mhtiAxFilesParent == NULL)
	{
		CString sText;
		sText = theWorkspace.LoadResourceString(IDS_AXFILEPARENT);
		mhtiAxFilesParent = InsertItem(sText, TVI_ROOT, TVI_SORT);
		SetItemImage(mhtiAxFilesParent, 1,1);
	}	
	
	// add the child item now.
	HTREEITEM htiTreeItem = InsertItem(sFileName, mhtiAxFilesParent, TVI_SORT);
	SetItemImage(htiTreeItem, 8,8);
}

void CProjectTreeCtrl::AddFormToTree(CDclFormObject *pDcl, bool bForceShow)
{
	CString sText;
	
	if (pDcl != NULL)
	{
		switch (pDcl->GetType())
		{
			
		case VdclFileDialog:
			if (mhtiFileDialogParent == NULL)
			{
				sText = theWorkspace.LoadResourceString(IDS_FILEDLG);
				mhtiFileDialogParent = InsertItem(sText, TVI_ROOT, TVI_SORT);
				SetItemImage(mhtiFileDialogParent, 1,1);
			}				
			pDcl->m_htiTreeItem = InsertItem(pDcl->GetKeyName(), mhtiFileDialogParent, TVI_SORT);
			SetItemImage(pDcl->m_htiTreeItem, 9,9);			
			break;

		case VdclModal:
			if (mhtiModalParent == NULL)
			{
				sText = theWorkspace.LoadResourceString(IDS_MODALFORM);
				mhtiModalParent = InsertItem(sText, TVI_ROOT, TVI_SORT);
				SetItemImage(mhtiModalParent, 1,1);
			}				
			pDcl->m_htiTreeItem = InsertItem(pDcl->GetKeyName(), mhtiModalParent, TVI_SORT);
			SetItemImage(pDcl->m_htiTreeItem, 2,2);			
			break;
		case VdclModeless:
			if (mhtiModelessParent == NULL)
			{
				sText = theWorkspace.LoadResourceString(IDS_MODELESSFORM);
				mhtiModelessParent = InsertItem(sText, TVI_ROOT, TVI_SORT);
				SetItemImage(mhtiModelessParent, 1,1);
			}
			pDcl->m_htiTreeItem = InsertItem(pDcl->GetKeyName(), mhtiModelessParent, TVI_SORT);
			SetItemImage(pDcl->m_htiTreeItem, 3,3);
			break;
		case VdclDockable:
			if (mhtiDockableParent == NULL)
			{
				sText = theWorkspace.LoadResourceString(IDS_DOCKABLEFORM);
				mhtiDockableParent = InsertItem(sText, TVI_ROOT, TVI_SORT);
				SetItemImage(mhtiDockableParent, 1,1);
			}
			pDcl->m_htiTreeItem = InsertItem(pDcl->GetKeyName(), mhtiDockableParent, TVI_SORT);
			SetItemImage(pDcl->m_htiTreeItem, 4,4);
			break;
		case VdclConfigTab:
			if (mhtiConfigParent == NULL)
			{
				sText = theWorkspace.LoadResourceString(IDS_CONFIGTAB);
				mhtiConfigParent = InsertItem(sText, TVI_ROOT, TVI_SORT);
				SetItemImage(mhtiConfigParent, 1,1);
			}
			pDcl->m_htiTreeItem = InsertItem(pDcl->GetKeyName(), mhtiConfigParent, TVI_SORT);
			SetItemImage(pDcl->m_htiTreeItem, 5,5);
			break;
		case VdclTabForm:
			HTREEITEM hItem = FindTabParent(pDcl);
			if (hItem != NULL && hItem != TVI_ROOT)
			{
				pDcl->m_htiTreeItem = InsertItem(FindTabCaption(pDcl), hItem);
				SetItemImage(pDcl->m_htiTreeItem, 5,5);
			}
			break;
		}
		// if the new tree node is to be shown
		if (bForceShow)
		{			
			EnsureVisible(pDcl->m_htiTreeItem);
			SetFocus();
			SelectItem(pDcl->m_htiTreeItem);
		}
	}	
}


HTREEITEM CProjectTreeCtrl::FindTabParent(CDclFormObject *pDclTab)
{
	// do loop to add all the tree items
	for (int i=0; i<mpProject->GetDclFormList().GetCount(); i++)
	{
		POSITION pos = mpProject->GetDclFormList().FindIndex(i);
		if (pos != NULL)
		{
			CDclFormObject *pDcl = mpProject->GetDclFormList().GetAt(pos);
			if (pDcl != NULL)
			{
				if (pDclTab->m_ParentName == pDcl->m_UniqueName)
				{
					return pDcl->m_htiTreeItem;
				}
			}			
		}
	}

	return NULL;
}

void CProjectTreeCtrl::CleanupParents()
{
	if (GetChildItem(mhtiFileDialogParent) == NULL)
	{
		DeleteItem(mhtiFileDialogParent);
		mhtiFileDialogParent = NULL;
	}
	if (GetChildItem(mhtiModalParent) == NULL)
	{
		DeleteItem(mhtiModalParent);
		mhtiModalParent = NULL;
	}
	if (GetChildItem(mhtiModelessParent) == NULL)
	{
		DeleteItem(mhtiModelessParent);
		mhtiModelessParent = NULL;
	}
	if (GetChildItem(mhtiDockableParent) == NULL)
	{
		DeleteItem(mhtiDockableParent);
		mhtiDockableParent = NULL;
	}
	if (GetChildItem(mhtiConfigParent) == NULL)
	{
		DeleteItem(mhtiConfigParent);
		mhtiConfigParent = NULL;
	}
	if (GetChildItem(mhtiAxFilesParent) == NULL)
	{
		DeleteItem(mhtiAxFilesParent);
		mhtiAxFilesParent = NULL;
	}	
}

void CProjectTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (point.x == nNotSet && point.y == nNotSet)
		point = (CPoint) GetMessagePos();

	ScreenToClient(&point);

	UINT uFlags;
	HTREEITEM htItem;
	
	htItem = HitTest( point, &uFlags );

	if( htItem == NULL )
		return;
	
	CMenu menu;
	CMenu* pPopup;

	// the font popup is stored in a resource
	switch (nCurrentPurchaseMode)
		{
		case nPurchasedLT:
			menu.LoadMenu(IDR_POPUPMENUS_LT);
			break;
		case nPurchasedGMP:
		case nPurchasedR14Pro:
			menu.LoadMenu(IDR_POPUPMENUS_R14);
			break;
		case nPurchasedStd:
			menu.LoadMenu(IDR_POPUPMENUS_STD);
			break;
		default:
			menu.LoadMenu(IDR_POPUPMENUS);
			break;
		}
		
	pPopup = menu.GetSubMenu(0);
	ClientToScreen(&point);
	CObjectDCLApp* pApp = (CObjectDCLApp*)AfxGetApp();
	pPopup->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, pApp->m_pMainWnd );	
}




void CProjectTreeCtrl::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// get the selected item
	TV_ITEM SelectedItem = pNMTreeView->itemNew;
	if (GetParentItem(SelectedItem.hItem) == NULL)
	{
		if (pNMTreeView->action == 2)
			SetItemImage(SelectedItem.hItem, 0,0);
		else
			SetItemImage(SelectedItem.hItem, 1,1);
	}
	*pResult = 0;
}

BOOL CProjectTreeCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message== WM_KEYDOWN )
	{
		if (pMsg->wParam==VK_RETURN)
		{
			pMsg->wParam = NULL;
			pMsg->message = NULL;
		}
	}		
		
	return CTreeCtrl::PreTranslateMessage(pMsg);
}



void CProjectTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	
	CTreeCtrl::OnRButtonDown(nFlags, point);

	HTREEITEM hItem = GetSelectedItem();
	CString sText = GetItemText(hItem);
	HTREEITEM hHitItem = HitTest(point, &nFlags);

	if (hHitItem != NULL)
		SelectItem(hHitItem);
	// Send WM_CONTEXTMENU to self
	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
	
}
