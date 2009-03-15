// ProjectPane.cpp : implementation file
//

#include "stdafx.h"
#include "ProjectPane.h"
#include "StudioWorkspace.h"
#include "OpenDCL.h"
#include "StudioFrame.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "OpenDCLDoc.h"
#include "ControlTypes.h"
#include "PropertyIds.h"
#include "Resource.h"
#include "PictureFolder.h"
#include "ProjectPasswordDlg.h"
#include "DclFormView.h"
#include "ControlManager.h"
#include "StudioDialogControl.h"


static CString StripPathFromFileName(CString sFullPath)
{
	CString sShortName = sFullPath;
	sShortName.MakeReverse();
	sShortName = sShortName.SpanExcluding(_T("\\/:"));
	sShortName.MakeReverse();
	return sShortName;
}


static CString FindTabCaption2(TDclFormPtr pDclTab, int nTabIndex)
{
	TDclControlPtr pControl = pDclTab->FindFirstControlOfType(CtlTabStrip);
	if (pControl)
		return pControl->GetPropertyListItem(Prop::TabsCaption, nTabIndex);
	return CString();
}


static CString FindTabCaption(TDclFormPtr pDclTabPage)
{
	const TDclFormList& Forms = pDclTabPage->GetProject()->GetDclFormList();
	for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
	{
		if( pDclTabPage->GetParentName() == (*iter)->GetUniqueName() )
			return FindTabCaption2( (*iter), pDclTabPage->GetTabIndex() );
	}
	return CString();
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPane

BEGIN_MESSAGE_MAP(CProjectPane, CCtrlView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_REMOVEFORM, &CProjectPane::OnRemoveForm)
	ON_UPDATE_COMMAND_UI(ID_REMOVEFORM, &CProjectPane::OnUpdateRemoveForm)
	ON_COMMAND(ID_ADDMODAL, &CProjectPane::OnAddmodal)
	ON_UPDATE_COMMAND_UI(ID_ADDMODAL, &CProjectPane::OnUpdateAddmodal)
	ON_COMMAND(ID_ADDMODELESS, &CProjectPane::OnAddmodeless)
	ON_UPDATE_COMMAND_UI(ID_ADDMODELESS, &CProjectPane::OnUpdateAddmodeless)
	ON_COMMAND(ID_ADDCONTROLBAR, &CProjectPane::OnAddcontrolbar)
	ON_UPDATE_COMMAND_UI(ID_ADDCONTROLBAR, &CProjectPane::OnUpdateAddcontrolbar)
	ON_COMMAND(ID_ADDOPTIONS, &CProjectPane::OnAddconfig)
	ON_UPDATE_COMMAND_UI(ID_ADDOPTIONS, &CProjectPane::OnUpdateAddconfig)
	ON_COMMAND(ID_ADDFILEDIALOG, &CProjectPane::OnAddfiledialogbox)
	ON_UPDATE_COMMAND_UI(ID_ADDFILEDIALOG, &CProjectPane::OnUpdateAddfiledialogbox)
	ON_COMMAND(ID_ADDPALETTE, &CProjectPane::OnAddpalette)
	ON_UPDATE_COMMAND_UI(ID_ADDPALETTE, &CProjectPane::OnUpdateAddpalette)
	ON_COMMAND(ID_PICTUREFOLDER, &CProjectPane::OnPicturefolder)
	ON_UPDATE_COMMAND_UI(ID_PICTUREFOLDER, &CProjectPane::OnUpdatePicturefolder)
	ON_COMMAND(ID_SETPROJECTPASSWORD, &CProjectPane::OnSetpassword)
	ON_UPDATE_COMMAND_UI(ID_SETPROJECTPASSWORD, &CProjectPane::OnUpdateSetpassword)
	ON_COMMAND(ID_SETAUTOLISPFILENAME, &CProjectPane::OnSetautolispfilename)
	ON_UPDATE_COMMAND_UI(ID_SETAUTOLISPFILENAME, &CProjectPane::OnUpdateSetautolispfilename)
END_MESSAGE_MAP()


IMPLEMENT_DYNCREATE(CProjectPane, CCtrlView)

CProjectPane::CProjectPane()
: CCtrlView( WC_TREEVIEW, WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT|TVS_SHOWSELALWAYS )
, mpProject( NULL )
, mhtiModalParent( NULL )
, mhtiModelessParent( NULL )
, mhtiControlBarParent( NULL )
, mhtiOptionsTabParent( NULL )
, mhtiFileDialogParent( NULL )
, mhtiPaletteParent( NULL )
, mhtiAutoLispFileParent( NULL )
, mhtiAutoLispFile( NULL )
, mhtiPasswordParent( NULL )
, mhtiPassword( NULL )
, mhtiAxFilesParent( NULL )
{
	mTreeImageList.Create( 16, 16, ILC_COLOR4 | ILC_MASK, 11, 0 );
	
	HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_OPENFOLDER), RT_GROUP_ICON);
	mTreeImageList.Add(LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_OPENFOLDER)));
	mTreeImageList.Add(LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_CLOSEDFOLDER)));
	mTreeImageList.Add(LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_MODAL)));
	mTreeImageList.Add(LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_MODELESS)));
	mTreeImageList.Add(LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_CONTROLBAR)));
	mTreeImageList.Add(LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_OPTIONSTAB)));
	mTreeImageList.Add(LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_LSP)));
	mTreeImageList.Add(LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_PASSWORD)));
	mTreeImageList.Add(LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_AXFILE)));
	mTreeImageList.Add(LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_FILEDLG)));
	mTreeImageList.Add(LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_PALETTE)));
}

CProjectPane::~CProjectPane()
{
}

bool CProjectPane::IsFormSelected() const
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if( !hItem )
		return false;
	const TDclFormList& Forms = mpProject->GetDclFormList();
	for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
	{
		if( (*iter)->m_htiTreeItem == hItem )
			return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CProjectPane message handlers

int CProjectPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	mTreeImageList.SetBkColor(GetTreeCtrl().GetBkColor());
	GetTreeCtrl().SetImageList(&mTreeImageList, TVSIL_NORMAL);
	
	return 0;
}

BOOL CProjectPane::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	//// include the dialog object in the command routing chain if this view is the active view
	//if( theStudioWorkspace.GetStudioFrame()->GetActiveView() == this )
	//{
	//	CDclFormView* pFormView = theStudioWorkspace.GetActiveFormView();
	//	if( pFormView )
	//	{
	//		if( pFormView->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
	//			return TRUE;
	//	}
	//}

	return __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CProjectPane::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
		CDclFormView* pFormView = theStudioWorkspace.GetActiveFormView();
		if( pFormView )
		{
			theStudioWorkspace.GetStudioFrame()->SetActiveView( pFormView );
			return;
		}
	__super::OnActivateView( bActivate, pActivateView, pDeactiveView );
}

void CProjectPane::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem == NULL)
		return;// exit out if the user did not double click on a item
	
	if (mhtiAutoLispFile == hItem)
	{
		// update the project tree's autolisp file name
		CString sLispFileName = mpProject->GetLispFileName();
		CFileDialog Dlg( FALSE,
										 _T(".lsp"), 
										 sLispFileName, 
										 OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
										 theWorkspace.LoadResourceString(IDS_FILTERAUTOLISPFILE),
										 CWnd::GetActiveWindow() );
		CString sTitle = theWorkspace.LoadResourceString(IDS_SELECTPROJECTLISPFILE);
		Dlg.m_pOFN->lpstrTitle = sTitle.LockBuffer();
		if( Dlg.DoModal() == IDOK )
		{
			sLispFileName = Dlg.GetPathName();
			mpProject->SetLispFileName( sLispFileName );
			SetAutoLispFilename( sLispFileName );
		}
		*pResult = 1;
		return;
	}
	
	if (mhtiPassword == hItem)
	{
		// update the project's password
		CString sPassword = mpProject->GetPassword();
		CProjectPasswordDlg Dlg( sPassword, this );
		if( Dlg.DoModal() == IDOK )
		{
			sPassword = Dlg.GetPassword();
			mpProject->SetPassword( sPassword );
  		SetPassword( sPassword );
		}
		*pResult = 1;
		return;
	}

	assert( mpProject != NULL );
	const TDclFormList& Forms = mpProject->GetDclFormList();
	for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
	{
		if( (*iter)->m_htiTreeItem == hItem )
		{
			mpProject->OpenDclFormView( (*iter) );
			*pResult = 1;
			return;
		}
	}
}

void CProjectPane::RemoveChildren(HTREEITEM hParent)
{
	if( !hParent )
		return;
	HTREEITEM hChild = GetTreeCtrl().GetChildItem( hParent );
	while (hChild != NULL)
	{
		GetTreeCtrl().DeleteItem( hChild );
		hChild = GetTreeCtrl().GetChildItem( hParent );
	}
}

void CProjectPane::SetAutoLispFilename( LPCTSTR pszLispFilename )
{
	CString sLispFilename = pszLispFilename;
	if( sLispFilename.IsEmpty() )
		sLispFilename = theWorkspace.LoadResourceString(IDS_NONE);
	GetTreeCtrl().SetItemText(mhtiAutoLispFile, sLispFilename);
}

void CProjectPane::SetPassword( LPCTSTR pszPassword )
{
	CString sPassword = pszPassword;
	if( sPassword.IsEmpty() )
		sPassword = theWorkspace.LoadResourceString(IDS_NONE);
	GetTreeCtrl().SetItemText(mhtiPassword, sPassword);
}

void CProjectPane::OnActivateProject(TStudioProjectPtr pProject /*= NULL*/)
{
	if( m_hWnd )
		GetTreeCtrl().DeleteAllItems();
	mhtiAutoLispFileParent = NULL;
	mhtiPasswordParent = NULL;
	mhtiModalParent = NULL;
	mhtiModelessParent = NULL;
	mhtiControlBarParent = NULL;
	mhtiOptionsTabParent = NULL;
	mhtiAutoLispFile = NULL;
	mhtiPassword = NULL;
	mhtiAxFilesParent = NULL;
	mhtiFileDialogParent = NULL;
	mhtiPaletteParent = NULL;

	if( mpProject && mpProject != pProject )
	{
		CDocument* pDoc = GetDocument();
		if( pDoc )
		{
			CFrameWnd* pMainFrame = theStudioWorkspace.GetStudioFrame();
			CView* pActiveView = pMainFrame->GetActiveView();
			POSITION pos = pDoc->GetFirstViewPosition();
			while( pos )
			{
				CView* pView = pDoc->GetNextView( pos );
				if( pView != this )
				{
					if( pView == pActiveView )
						pMainFrame->SetActiveView( this );
					pView->GetParentFrame()->DestroyWindow();
				}
			}
		}
	}

	mpProject = pProject;
	if(mpProject)
	{
		CString sAppPrefix;
		GetDocument()->GetDocTemplate()->GetDocString( sAppPrefix, CDocTemplate::windowTitle );
		if( GetDocument()->IsModified() )
			sAppPrefix += _T('*');
		CStudioFrame* pStudioFrame = theStudioWorkspace.GetStudioFrame();
		pStudioFrame->SetWindowText( sAppPrefix + _T(" - ") + mpProject->GetDocument()->GetTitle() );
		pStudioFrame->DelayUpdateFrameTitle();
		const TDclFormList& Forms = mpProject->GetDclFormList();
		for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
			AddFormToTree( (*iter), false );
	}
	else
	{
		//CStudioFrame* pStudioFrame = theStudioWorkspace.GetStudioFrame();
		//pStudioFrame->SetWindowText( _T("") );
		//pStudioFrame->DelayUpdateFrameTitle();
		return;
	}
	
	CString sText;
	sText = theWorkspace.LoadResourceString(IDS_LSPFILENAME);
	mhtiAutoLispFileParent = GetTreeCtrl().InsertItem(sText, TVI_ROOT, TVI_SORT);
	GetTreeCtrl().SetItemImage(mhtiAutoLispFileParent, 1,1);
	if( mpProject )
		sText = mpProject->GetLispFileName();
	if( sText.IsEmpty() )
		sText = theWorkspace.LoadResourceString(IDS_NONE);
	mhtiAutoLispFile = GetTreeCtrl().InsertItem(sText, mhtiAutoLispFileParent);
	GetTreeCtrl().SetItemImage(mhtiAutoLispFile, 6,6);	

	sText = theWorkspace.LoadResourceString(IDS_PASSWORD);
	mhtiPasswordParent = GetTreeCtrl().InsertItem(sText, TVI_ROOT, TVI_SORT);
	GetTreeCtrl().SetItemImage(mhtiPasswordParent, 1,1);
	if( mpProject )
		sText = mpProject->GetPassword();
	if( sText.IsEmpty() )
		sText = theWorkspace.LoadResourceString(IDS_NONE);
	mhtiPassword = GetTreeCtrl().InsertItem(sText, mhtiPasswordParent);
}

void CProjectPane::OnActivateForm( TDclFormPtr pForm /*= NULL*/ )
{
	if( pForm )
	{
		GetTreeCtrl().EnsureVisible( pForm->m_htiTreeItem );
		GetTreeCtrl().SelectItem( pForm->m_htiTreeItem );
	}
}

void CProjectPane::AddFormToTree( TDclFormPtr pDcl, bool bForceShow )
{
	CString sText;
	
	if (pDcl != NULL)
	{
		switch (pDcl->GetType())
		{
			
		case FrmFileDlg:
			if (mhtiFileDialogParent == NULL)
			{
				sText = theWorkspace.LoadResourceString(IDS_FILEDLGS);
				mhtiFileDialogParent = GetTreeCtrl().InsertItem(sText, TVI_ROOT, TVI_SORT);
				GetTreeCtrl().SetItemImage(mhtiFileDialogParent, 1,1);
			}				
			pDcl->m_htiTreeItem = GetTreeCtrl().InsertItem(pDcl->GetKeyName(), mhtiFileDialogParent, TVI_SORT);
			GetTreeCtrl().SetItemImage(pDcl->m_htiTreeItem, 9,9);			
			break;

		case FrmModalDlg:
			if (mhtiModalParent == NULL)
			{
				sText = theWorkspace.LoadResourceString(IDS_MODALFORMS);
				mhtiModalParent = GetTreeCtrl().InsertItem(sText, TVI_ROOT, TVI_SORT);
				GetTreeCtrl().SetItemImage(mhtiModalParent, 1,1);
			}				
			pDcl->m_htiTreeItem = GetTreeCtrl().InsertItem(pDcl->GetKeyName(), mhtiModalParent, TVI_SORT);
			GetTreeCtrl().SetItemImage(pDcl->m_htiTreeItem, 2,2);
			break;
		case FrmModelessDlg:
			if (mhtiModelessParent == NULL)
			{
				sText = theWorkspace.LoadResourceString(IDS_MODELESSFORMS);
				mhtiModelessParent = GetTreeCtrl().InsertItem(sText, TVI_ROOT, TVI_SORT);
				GetTreeCtrl().SetItemImage(mhtiModelessParent, 1,1);
			}
			pDcl->m_htiTreeItem = GetTreeCtrl().InsertItem(pDcl->GetKeyName(), mhtiModelessParent, TVI_SORT);
			GetTreeCtrl().SetItemImage(pDcl->m_htiTreeItem, 3,3);
			break;
		case FrmControlBar:
			if (mhtiControlBarParent == NULL)
			{
				sText = theWorkspace.LoadResourceString(IDS_CONTROLBARS);
				mhtiControlBarParent = GetTreeCtrl().InsertItem(sText, TVI_ROOT, TVI_SORT);
				GetTreeCtrl().SetItemImage(mhtiControlBarParent, 1,1);
			}
			pDcl->m_htiTreeItem = GetTreeCtrl().InsertItem(pDcl->GetKeyName(), mhtiControlBarParent, TVI_SORT);
			GetTreeCtrl().SetItemImage(pDcl->m_htiTreeItem, 4,4);
			break;
		case FrmOptionsTab:
			if (mhtiOptionsTabParent == NULL)
			{
				sText = theWorkspace.LoadResourceString(IDS_OPTIONSTABS);
				mhtiOptionsTabParent = GetTreeCtrl().InsertItem(sText, TVI_ROOT, TVI_SORT);
				GetTreeCtrl().SetItemImage(mhtiOptionsTabParent, 1,1);
			}
			pDcl->m_htiTreeItem = GetTreeCtrl().InsertItem(pDcl->GetKeyName(), mhtiOptionsTabParent, TVI_SORT);
			GetTreeCtrl().SetItemImage(pDcl->m_htiTreeItem, 5,5);
			break;
		case FrmTabPage:
			{
				HTREEITEM hItem = FindTabParent( pDcl );
				if (hItem != NULL && hItem != TVI_ROOT)
				{
					HTREEITEM hInsertAfter = TVI_FIRST;
					short idxTab = pDcl->GetTabIndex();
					for( short idx = idxTab; idx > 0; --idx )
					{
						HTREEITEM hNext =
							(hInsertAfter == TVI_FIRST)? GetTreeCtrl().GetChildItem( hItem ) : GetTreeCtrl().GetNextSiblingItem( hInsertAfter );
						if( !hNext )
							break;
						if( GetTreeCtrl().GetItemData( hNext ) > (DWORD_PTR)idxTab )
							break;
						hInsertAfter = hNext;
					}
					if( !hInsertAfter )
						hInsertAfter = TVI_LAST;
					pDcl->m_htiTreeItem = GetTreeCtrl().InsertItem( FindTabCaption( pDcl ), hItem, hInsertAfter );
					GetTreeCtrl().SetItemImage( pDcl->m_htiTreeItem, 5, 5 );
					GetTreeCtrl().SetItemData( pDcl->m_htiTreeItem, pDcl->GetTabIndex() );
					GetTreeCtrl().Expand( hItem, TVE_EXPAND );
				}
			}
			break;
		case FrmPaletteDlg:
			if (mhtiPaletteParent == NULL)
			{
				sText = theWorkspace.LoadResourceString(IDS_PALETTEFORMS);
				mhtiPaletteParent = GetTreeCtrl().InsertItem(sText, TVI_ROOT, TVI_SORT);
				GetTreeCtrl().SetItemImage(mhtiPaletteParent, 1,1);
			}
			pDcl->m_htiTreeItem = GetTreeCtrl().InsertItem(pDcl->GetKeyName(), mhtiPaletteParent, TVI_SORT);
			GetTreeCtrl().SetItemImage(pDcl->m_htiTreeItem, 10, 10);
			break;
		}
		// if the new tree node is to be shown
		if (bForceShow)
		{			
			GetTreeCtrl().EnsureVisible(pDcl->m_htiTreeItem);
			SetFocus();
			GetTreeCtrl().SelectItem(pDcl->m_htiTreeItem);
		}
	}	
}

HTREEITEM CProjectPane::FindTabParent(TDclFormPtr pDclTab)
{
	TDclFormPtr pParentForm = pDclTab->GetParentForm();
	assert( pParentForm != NULL );
	if( !pParentForm )
		return NULL;
	return pParentForm->m_htiTreeItem;
}

void CProjectPane::CleanupParents()
{
	if (GetTreeCtrl().GetChildItem(mhtiFileDialogParent) == NULL)
	{
		GetTreeCtrl().DeleteItem(mhtiFileDialogParent);
		mhtiFileDialogParent = NULL;
	}
	if (GetTreeCtrl().GetChildItem(mhtiModalParent) == NULL)
	{
		GetTreeCtrl().DeleteItem(mhtiModalParent);
		mhtiModalParent = NULL;
	}
	if (GetTreeCtrl().GetChildItem(mhtiModelessParent) == NULL)
	{
		GetTreeCtrl().DeleteItem(mhtiModelessParent);
		mhtiModelessParent = NULL;
	}
	if (GetTreeCtrl().GetChildItem(mhtiControlBarParent) == NULL)
	{
		GetTreeCtrl().DeleteItem(mhtiControlBarParent);
		mhtiControlBarParent = NULL;
	}
	if (GetTreeCtrl().GetChildItem(mhtiOptionsTabParent) == NULL)
	{
		GetTreeCtrl().DeleteItem(mhtiOptionsTabParent);
		mhtiOptionsTabParent = NULL;
	}
	if (GetTreeCtrl().GetChildItem(mhtiPaletteParent) == NULL)
	{
		GetTreeCtrl().DeleteItem(mhtiPaletteParent);
		mhtiPaletteParent = NULL;
	}
	if (GetTreeCtrl().GetChildItem(mhtiAxFilesParent) == NULL)
	{
		GetTreeCtrl().DeleteItem(mhtiAxFilesParent);
		mhtiAxFilesParent = NULL;
	}	
}

void CProjectPane::OnRemoveForm() 
{
	ASSERT(mpProject != NULL);
	if( !mpProject )
		return;

	// get the selected form
	HTREEITEM hDclForm = GetTreeCtrl().GetSelectedItem();
	if( !hDclForm )
		return;
	
	TDclFormPtr pDclForm;
	TDclFormPtr pParentForm;
	HTREEITEM hParentItem = GetTreeCtrl().GetParentItem( hDclForm );
	if( hParentItem )
	{
		pParentForm = mpProject->FindDclForm( GetTreeCtrl().GetItemText( hParentItem ) );
		if( pParentForm )
		{
			size_t idxTab = GetTreeCtrl().GetItemData( hDclForm );
			pDclForm = mpProject->FindDclTabChildForm( pParentForm->GetUniqueName(), idxTab );
		}
	}
	else
		pDclForm = mpProject->FindDclForm( GetTreeCtrl().GetItemText( hDclForm ) );
	if( !pDclForm )
	{
		MessageBox( theWorkspace.LoadResourceString(IDS_DCLTREEITEMNOTSEL),
								theWorkspace.GetAppKey(),
								(MB_OK | MB_ICONEXCLAMATION) );
		return;
	}

	//prompt the user to make sure they want to delete the form
	if( MessageBox( theWorkspace.LoadResourceString(IDS_QDELETEFORM),
									theWorkspace.GetAppKey(),
									(MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) ) != IDYES )
		return;

	AutoUndoGroup UndoGroup( pDclForm->GetUndoManager(), IDS_UNDO_REMOVEFORM );
	assert(pDclForm->m_htiTreeItem == hDclForm);
	if( pParentForm )
	{ //need to delete the reference from the parent
		switch( pDclForm->GetType() )
		{
		case FrmTabPage:
			{
				HTREEITEM hSibling = hDclForm;
				while( (hSibling = GetTreeCtrl().GetNextSiblingItem( hSibling )) != NULL )
				{
					size_t idxTab = GetTreeCtrl().GetItemData( hSibling );
					GetTreeCtrl().SetItemData( hSibling, idxTab - 1 );
					TDclFormPtr pDclForm = mpProject->FindDclTabChildForm( pParentForm->GetUniqueName(), idxTab );
					if( pDclForm )
						pDclForm->SetTabIndex( idxTab - 1 );
				}
				TDclControlPtr pTabStrip = pParentForm->FindFirstControlOfType( CtlTabStrip );
				if( pTabStrip )
				{
					TPropertyPtr pCaptions = pTabStrip->GetPropertyObject( Prop::TabsCaption );
					if( pCaptions )
					{
						PropVal::TCStringArray* prCaption = pCaptions->GetStringArrayPtr();
						PropVal::TCStringArray::iterator iter = prCaption->begin();
						size_t idxTab = pDclForm->GetTabIndex();
						while( iter != prCaption->end() && idxTab > 0 ) { ++iter; --idxTab; }
						if( iter != prCaption->end() )
							prCaption->erase( iter );
					}
					TPropertyPtr pImages = pTabStrip->GetPropertyObject( Prop::TabsImageList );
					if( pImages )
					{
						PropVal::TIntArray* prImage = pImages->GetIntArrayPtr();
						PropVal::TIntArray::iterator iter = prImage->begin();
						size_t idxTab = pDclForm->GetTabIndex();
						while( iter != prImage->end() && idxTab > 0 ) { ++iter; --idxTab; }
						if( iter != prImage->end() )
							prImage->erase( iter );
					}
					TPropertyPtr pTooltips = pTabStrip->GetPropertyObject( Prop::TabsTTT );
					if( pTooltips )
					{
						PropVal::TCStringArray* prTooltip = pTooltips->GetStringArrayPtr();
						PropVal::TCStringArray::iterator iter = prTooltip->begin();
						size_t idxTab = pDclForm->GetTabIndex();
						while( iter != prTooltip->end() && idxTab > 0 ) { ++iter; --idxTab; }
						if( iter != prTooltip->end() )
							prTooltip->erase( iter );
					}
					const CDialogControl* pTabStripCtrl = pTabStrip->GetControlInstance();
					if( pTabStripCtrl )
					{
						CStudioDialogControl::UpdateAllProperties( pTabStrip );
						if( pTabStripCtrl->GetControlManager()->IsSelected() )
							theStudioWorkspace.ActivateDclControl( pTabStrip );
					}
				}
			}
			break;
		}
	}
	GetTreeCtrl().DeleteItem( hDclForm ); // delete the item.
	mpProject->DeleteForm( pDclForm ); //pDclForm is invalid after this call!

	CDocument* pDoc = GetDocument();
	if( pDoc )
		pDoc->SetModifiedFlag();

	CleanupParents();
}

void CProjectPane::OnUpdateRemoveForm(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( IsFormSelected() );
}

void CProjectPane::OnAddmodal() 
{
	theStudioWorkspace.GetStudioFrame()->AddNewDclFormView( FrmModalDlg );
}

void CProjectPane::OnUpdateAddmodal(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (mpProject != NULL) );
}

void CProjectPane::OnAddmodeless() 
{
	theStudioWorkspace.GetStudioFrame()->AddNewDclFormView( FrmModelessDlg );
}

void CProjectPane::OnUpdateAddmodeless(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (mpProject != NULL) );
}

void CProjectPane::OnAddconfig() 
{
	theStudioWorkspace.GetStudioFrame()->AddNewDclFormView( FrmOptionsTab );
}

void CProjectPane::OnUpdateAddconfig(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (mpProject != NULL) );
}

void CProjectPane::OnAddcontrolbar() 
{
	theStudioWorkspace.GetStudioFrame()->AddNewDclFormView( FrmControlBar );
}

void CProjectPane::OnUpdateAddcontrolbar(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (mpProject != NULL) );
}

void CProjectPane::OnAddfiledialogbox() 
{
	theStudioWorkspace.GetStudioFrame()->AddNewDclFormView( FrmFileDlg );
}

void CProjectPane::OnUpdateAddfiledialogbox(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (mpProject != NULL) );
}

void CProjectPane::OnAddpalette() 
{
	theStudioWorkspace.GetStudioFrame()->AddNewDclFormView( FrmPaletteDlg );
}

void CProjectPane::OnUpdateAddpalette(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (mpProject != NULL) );
}

void CProjectPane::OnPicturefolder() 
{
	CPictureFolder Dlg( mpProject, this );
	Dlg.DoModal();
}

void CProjectPane::OnUpdatePicturefolder(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (mpProject != NULL) );
}

void CProjectPane::OnSetautolispfilename() 
{
	CProjectPane* pProjTree = theStudioWorkspace.GetProjectPane();
	ASSERT(pProjTree != NULL);
	if (!pProjTree)
		return;
	TStudioProjectPtr pProject = pProjTree->GetProject();
	ASSERT(pProject != NULL);
	if (!pProject)
		return;

	CString sLispFileName = pProject->GetLispFileName();
	CFileDialog Dlg( FALSE,
									 _T(".lsp"), 
									 sLispFileName, 
									 OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
									 theWorkspace.LoadResourceString(IDS_FILTERAUTOLISPFILE),
									 CWnd::GetActiveWindow() );
	CString sTitle = theWorkspace.LoadResourceString(IDS_SELECTPROJECTLISPFILE);
	Dlg.m_pOFN->lpstrTitle = sTitle;
	if( Dlg.DoModal() == IDOK )
	{
		sLispFileName = Dlg.GetPathName();
		pProject->SetLispFileName( sLispFileName );
		pProjTree->SetAutoLispFilename( sLispFileName );
		CDocument* pDoc = pProjTree->GetDocument();
		if( pDoc )
			pDoc->SetModifiedFlag();
	}
}

void CProjectPane::OnUpdateSetautolispfilename(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (mpProject != NULL) );
}

void CProjectPane::OnSetpassword() 
{
	CProjectPane* pProjTree = theStudioWorkspace.GetProjectPane();
	ASSERT(pProjTree != NULL);
	if (!pProjTree)
		return;
	TStudioProjectPtr pProject = pProjTree->GetProject();
	ASSERT(pProject != NULL);
	if (!pProject)
		return;
	CProjectPasswordDlg Dlg( pProject->GetPassword(), this );
	if( Dlg.DoModal() == IDOK )
	{
		CString sPassword = Dlg.GetPassword();
		pProject->SetPassword( sPassword );
  	pProjTree->SetPassword( sPassword );
		CDocument* pDoc = pProjTree->GetDocument();
		if( pDoc )
			pDoc->SetModifiedFlag();
	}
}

void CProjectPane::OnUpdateSetpassword(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (mpProject != NULL) );
}

void CProjectPane::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (point.x == -1 && point.y == -1)
		point = (CPoint)GetMessagePos();
	CMenu menu;
	menu.LoadMenu(IDR_MAINFRAME);
	CMenu* pPopup = menu.GetSubMenu(2);
	ClientToScreen(&point);
	ScreenToClient(&point);
	pPopup->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, GetParentFrame() );	
}

void CProjectPane::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// get the selected item
	TV_ITEM SelectedItem = pNMTreeView->itemNew;
	if (GetTreeCtrl().GetParentItem(SelectedItem.hItem) == NULL)
	{
		if (pNMTreeView->action == 2)
			GetTreeCtrl().SetItemImage(SelectedItem.hItem, 0,0);
		else
			GetTreeCtrl().SetItemImage(SelectedItem.hItem, 1,1);
	}
	*pResult = 0;
}

BOOL CProjectPane::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message== WM_KEYDOWN )
	{
		if (pMsg->wParam==VK_RETURN)
		{
			pMsg->wParam = NULL;
			pMsg->message = NULL;
		}
	}		
		
	return CCtrlView::PreTranslateMessage(pMsg);
}

void CProjectPane::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CCtrlView::OnRButtonDown(nFlags, point);
	HTREEITEM hHitItem = GetTreeCtrl().HitTest(point, &nFlags);
	if (hHitItem != NULL)
		GetTreeCtrl().SelectItem(hHitItem);
	// Send WM_CONTEXTMENU to self
	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
}

BOOL CProjectPane::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style &= ~(FWS_ADDTOTITLE | FWS_PREFIXTITLE);
	return __super::PreCreateWindow(cs);
}
