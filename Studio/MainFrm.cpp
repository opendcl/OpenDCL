// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MainFrm.h"
#include "Editor.h"
#include "Project.h"
#include "PictureFolder.h"
#include "DclFormObject.h"
#include "EditorWorkspace.h"
#include "ProjectPasswordDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_PICTUREFOLDER, OnPicturefolder)
	ON_COMMAND(ID_ADDMODAL, OnAddmodal)
	ON_COMMAND(ID_ADDMODELESS, OnAddmodeless)
	ON_COMMAND(ID_ADDCONFIG, OnAddconfig)
	ON_COMMAND(ID_ADDDOCKABLE, OnAdddockable)
	ON_UPDATE_COMMAND_UI(ID_ADDMODAL, &CMainFrame::OnUpdateAddmodal)
	ON_UPDATE_COMMAND_UI(ID_ADDMODELESS, &CMainFrame::OnUpdateAddmodeless)
	ON_UPDATE_COMMAND_UI(ID_ADDDOCKABLE, &CMainFrame::OnUpdateAdddockable)
	ON_UPDATE_COMMAND_UI(ID_ADDCONFIG, &CMainFrame::OnUpdateAddconfig)
	ON_UPDATE_COMMAND_UI(ID_PICTUREFOLDER, &CMainFrame::OnUpdatePicturefolder)
	ON_COMMAND(ID_SETAUTOLISPFILENAME, OnSetautolispfilename)
	ON_COMMAND(ID_REMOVE, OnRemove)
	ON_COMMAND(ID_DOCK_PROJECT, OnDockProject)
	ON_UPDATE_COMMAND_UI(ID_DOCK_PROJECT, OnUpdateDockProject)
	ON_COMMAND(ID_DOCK_PROPERTIES, OnDockProperties)
	ON_UPDATE_COMMAND_UI(ID_DOCK_PROPERTIES, OnUpdateDockProperties)
	ON_COMMAND(ID_DOCK_TOOLBOX, OnDockToolbox)
	ON_UPDATE_COMMAND_UI(ID_DOCK_TOOLBOX, OnUpdateDockToolbox)
	ON_COMMAND(ID_DOCK_ZORDER, OnDockZorder)
	ON_UPDATE_COMMAND_UI(ID_DOCK_ZORDER, OnUpdateDockZorder)
	ON_COMMAND(ID_SETDISTFILENAME, OnSetpassword)
	ON_COMMAND(ID_PROJECTS_ADDFILEDIALOGBOX, OnProjectsAddfiledialogbox)
	ON_WM_SHOWWINDOW()
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CString sText;
	
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rcRet;
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);

	rcRet.left = pApp->GetProfileInt(sProfileName, _T("nTopLeftX"), -1);
  rcRet.top = pApp->GetProfileInt(sProfileName, _T("nTopLeftY"), -1);
	rcRet.right = rcRet.left + pApp->GetProfileInt(sProfileName, _T("sizeWidth"), -1);
	rcRet.bottom = rcRet.top + pApp->GetProfileInt(sProfileName, _T("sizeHeight"), -1);

	if (rcRet.right > 0)
		MoveWindow(rcRet, TRUE);

	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	
	if (!m_wndDlgBar.Create(this, IDR_FONTBTNS, CBRS_ALIGN_TOP, 2))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}

	// setup the tool bar for drop down buttons
	m_wndToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	TBBUTTONINFO tbi;
	tbi.dwMask= TBIF_STYLE;
	tbi.cbSize= sizeof(TBBUTTONINFO);
	m_wndToolBar.GetToolBarCtrl().GetButtonInfo(ID_ADDMODAL, &tbi);
	tbi.fsStyle |= TBSTYLE_DROPDOWN;
	m_wndToolBar.GetToolBarCtrl().SetButtonInfo(ID_ADDMODAL, &tbi);
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	
	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))		
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}
	EnableDocking(CBRS_ALIGN_ANY);

	// call method to create the properties bar
	CreatePropertyDockingBar();
	// call method to create the project bar
	CreateProjectDockingBar();
	// call method to create the toolbox bar
	CreateToolBoxBar();
	// call method to create the ZOrder bar
	CreateZOrderDockingBar();


#ifdef _SCB_REPLACE_MINIFRAME
    m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
#endif //_SCB_REPLACE_MINIFRAME

	RecalcLayout();

	DockControlBar(&m_wndProjectTreeBar, AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_wndToolBoxBar, AFX_IDW_DOCKBAR_LEFT);

	CRect rBar;
	m_wndToolBoxBar.GetWindowRect(rBar);
	rBar.OffsetRect(0, 1);
	DockControlBar(&m_wndZOderBar, AFX_IDW_DOCKBAR_LEFT, rBar);

	m_wndProjectTreeBar.GetWindowRect(rBar);	
	rBar.OffsetRect(1, 1);
	DockControlBar(&m_wndPropertyBar, AFX_IDW_DOCKBAR_RIGHT, rBar);

	CString sProfile = _T("BarState");
	if (VerifyBarState(sProfile))
	{
		CSizingControlBar::GlobalLoadState(this, sProfile);
		LoadBarState(sProfile);
	}

	return 0;
 }

void CMainFrame::CreateToolBoxBar()
{
	// --- project bar ---
	if (!m_wndToolBoxBar.Create(_T("Instant Bar"), this, 127))
	{
		TRACE0("Failed to create instant bar\n");
		return;		// fail to create
	}
	CString sText;
	sText = theWorkspace.LoadResourceString(IDS_TOOLBOX);
	m_wndToolBoxBar.SetWindowText(sText);
	m_wndToolBoxBar.SetSCBStyle(m_wndToolBoxBar.GetSCBStyle() | SCBS_SIZECHILD);

	m_ToolBox.Create(IDD_OPENDCL_FORM, &m_wndToolBoxBar);
	m_ToolBox.ShowWindow(TRUE);
	
	// --- end instant bar creation and child setup ---
	m_wndToolBoxBar.SetBarStyle(m_wndToolBoxBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
	
	// enable docking only on the left or right sides (or floating)
	m_wndToolBoxBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
}

void CMainFrame::CreateProjectDockingBar(TEditorProjectPtr pProject /*= NULL*/)
{
	// --- project bar ---
	if (!m_wndProjectTreeBar.Create(_T("Instant Bar"), this, 128))
	{
		TRACE0("Failed to create instant bar\n");
		return;		// fail to create
	}
	CString sText;
	sText = theWorkspace.LoadResourceString(IDS_PROJECT);
	m_wndProjectTreeBar.SetWindowText(sText);
	m_wndProjectTreeBar.SetSCBStyle(m_wndProjectTreeBar.GetSCBStyle() |
		SCBS_SIZECHILD);
	if (!m_ProjectTree.Create(WS_CHILD|WS_VISIBLE|
		TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT|TVS_SHOWSELALWAYS,
		CRect(0, 0, 0, 0), &m_wndProjectTreeBar, 100))
	{
		TRACE0("Failed to create instant bar child\n");
		return;		// fail to create
	}
	m_ProjectTree.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	COLORREF maskColor = RGB(0,255,0);
	// create and populate the image list for the project tree
	m_TreeImageList.Create(16,16,ILC_COLOR | ILC_MASK, 7, 0);
	
	HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_OPENFOLDER), RT_GROUP_ICON);

	HICON hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_OPENFOLDER));
	m_TreeImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_CLOSEDFOLDER));
	m_TreeImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_MODAL));
	m_TreeImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_MODELESS));
	m_TreeImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_DOCKABLE));
	m_TreeImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_TABFOLDER));
	m_TreeImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_LSP));
	m_TreeImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_PASSWORD));
	m_TreeImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_AXFILE));
	m_TreeImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_FILEDLG));
	m_TreeImageList.Add(hicon);
	DestroyIcon(hicon);

	m_TreeImageList.SetBkColor(m_ProjectTree.GetBkColor());
	m_ProjectTree.SetImageList(&m_TreeImageList, TVSIL_NORMAL);

	// bring the tooltips to front
	CWnd* pTT = FromHandle((HWND) m_ProjectTree.SendMessage(TVM_GETTOOLTIPS));
    if (pTT != NULL)
		pTT->SetWindowPos(&wndTopMost, 0, 0, 0, 0,
		SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

	// populate the treectrl
	m_ProjectTree.SetupProjectTree(pProject);
	
	// --- end instant bar creation and child setup ---
	m_wndProjectTreeBar.SetBarStyle(m_wndProjectTreeBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// enable docking only on the left of right sides (or floating)
	m_wndProjectTreeBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);

}

void CMainFrame::CreateZOrderDockingBar()
{
	// --- project bar ---
	if (!m_wndZOderBar.Create(_T("Instant Bar"), this, 130 ))
	{
		TRACE0("Failed to create instant bar\n");
		return;		// fail to create
	}

	CString sText;
	sText = theWorkspace.LoadResourceString(IDS_ZTABORDER);
	m_wndZOderBar.SetWindowText(sText);

	m_wndZOderBar.SetSCBStyle(m_wndZOderBar.GetSCBStyle() 
		| SCBS_SIZECHILD);
	
	m_ZOrderPane.Create(IDD_OPENDCL_FORM, &m_wndZOderBar);
	m_ZOrderPane.ShowWindow(TRUE);

	// --- end instant bar creation and child setup ---
	m_wndZOderBar.SetBarStyle(m_wndProjectTreeBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// enable docking only on the left of right sides (or floating)
	m_wndZOderBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	

}
void CMainFrame::CreatePropertyDockingBar()
{
	// --- project bar ---
	if (!m_wndPropertyBar.Create(_T("Instant Bar"), this, 131 ))
	{
		TRACE0("Failed to create instant bar\n");
		return;		// fail to create
	}
	m_wndPropertyBar.SetWindowText(theWorkspace.LoadResourceString(IDS_PROPERTIES));
	m_wndPropertyBar.SetSCBStyle(m_wndPropertyBar.GetSCBStyle() | SCBS_SIZECHILD);

	m_PropertyTabPane.Create(IDD_OPENDCL_FORM, &m_wndPropertyBar);
	m_PropertyTabPane.ShowWindow(TRUE);

	// --- end instant bar creation and child setup ---
	m_wndPropertyBar.SetBarStyle(m_wndProjectTreeBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// enable docking only on the left of right sides (or floating)
	m_wndPropertyBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= WS_VSCROLL|WS_HSCROLL;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


// This function is Copyright (c) 2000, Cristi Posea.
// See www.datamekanix.com for more control bars tips&tricks.
BOOL CMainFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
	CDockState state;
	state.LoadState(lpszProfileName);

	for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
	{
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
		ASSERT(pInfo != NULL);
		int nDockedCount = pInfo->m_arrBarID.GetSize();
		// dockbar
		for (int j = 0; j < nDockedCount; j++)
		{
			UINT nID = (UINT) pInfo->m_arrBarID[j];
			if (nID == 0) continue; // row separator
			if (nID > 0xFFFF)
					nID &= 0xFFFF; // placeholder - get the ID
			if (GetControlBar(nID) == NULL)
				return FALSE;
		}
		if (!pInfo->m_bFloating) // floating dockbars can be created later
			if (GetControlBar(pInfo->m_nBarID) == NULL)
				return FALSE; // invalid bar ID
	}

	return TRUE;
}

void CMainFrame::OnDestroy() 
{
	CWinApp* pApp = AfxGetApp();
	CRect rcThis;
	GetWindowRect(&rcThis);
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);

	pApp->WriteProfileInt(sProfileName, _T("sizeWidth"), rcThis.Width());
	pApp->WriteProfileInt(sProfileName, _T("sizeHeight"), rcThis.Height());

	pApp->WriteProfileInt(sProfileName, _T("nTopLeftX"), rcThis.left);
	pApp->WriteProfileInt(sProfileName, _T("nTopLeftY"), rcThis.top);


	CMDIFrameWnd::OnDestroy();
	
	m_TreeImageList.DeleteImageList();
	theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
}

BOOL CMainFrame::DestroyWindow() 
{
	CString sProfile = _T("BarState");
	CSizingControlBar::GlobalSaveState(this, sProfile);
	SaveBarState(sProfile);
	return CMDIFrameWnd::DestroyWindow();
}

void CMainFrame::OnPicturefolder() 
{
	CPictureFolder Dlg(m_ProjectTree.GetProject(), this);
	Dlg.DoModal();
}

void CMainFrame::OnAddmodal() 
{
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	if (!pProjTree)
		return;
	COpenDCLApp* pApp = (COpenDCLApp*)AfxGetApp();
	COpenDCLView *pView = pApp->AddDclFormAndView(VdclModal);
	CDocument* pDoc = pProjTree->GetDocument();
	if( pDoc )
		pDoc->SetModifiedFlag();
}

void CMainFrame::OnAddmodeless() 
{
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	if (!pProjTree)
		return;
	COpenDCLApp* pApp = (COpenDCLApp*)AfxGetApp();
	COpenDCLView *pView = pApp->AddDclFormAndView(VdclModeless);
	CDocument* pDoc = pProjTree->GetDocument();
	if( pDoc )
		pDoc->SetModifiedFlag();
}

void CMainFrame::OnAddconfig() 
{
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	if (!pProjTree)
		return;
	COpenDCLApp* pApp = (COpenDCLApp*)AfxGetApp();
	COpenDCLView *pView = pApp->AddDclFormAndView(VdclConfigTab);
	CDocument* pDoc = pProjTree->GetDocument();
	if( pDoc )
		pDoc->SetModifiedFlag();
}

void CMainFrame::OnAdddockable() 
{
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	if (!pProjTree)
		return;
	COpenDCLApp* pApp = (COpenDCLApp*)AfxGetApp();
	COpenDCLView *pView = pApp->AddDclFormAndView(VdclDockable);
	CDocument* pDoc = pProjTree->GetDocument();
	if( pDoc )
		pDoc->SetModifiedFlag();
}

BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	#define lpnm   ((LPNMHDR)lParam)
	#define lpnmTB ((LPNMTOOLBAR)lParam)

	switch(lpnm->code)
	{
	case TBN_DROPDOWN:
		//drop down button was hit
		CMenu menu;
		menu.LoadMenu(IDR_POPUPMENUS);
		CMenu* pPopup = menu.GetSubMenu(3);
		ASSERT(pPopup != NULL);
		
		CRect rc;
		m_wndToolBar.GetWindowRect(&rc);

		int x = rc.left;
		int y = rc.top + 22;
		
		pPopup->TrackPopupMenu(TPM_LEFTALIGN |	TPM_LEFTBUTTON, x, y, this);

		return FALSE; //indicates the TBN_DROPDOWN
                   //notification was handled.
	}
	return CMDIFrameWnd::OnNotify(wParam, lParam, pResult);
}

void CMainFrame::OnRemove() 
{
	// get the project tree
	CProjectTreeCtrl *pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	if (!pProjTree)
		return;
	TEditorProjectPtr pProject = pProjTree->GetProject();
	ASSERT(pProject != NULL);
	if (!pProject)
		return;

	// get the selected form
	HTREEITEM hDclForm = pProjTree->GetSelectedItem();
	HTREEITEM hParentItem = pProjTree->GetParentItem(hDclForm);

	if (hParentItem != NULL && pProjTree->GetParentItem(hParentItem) != NULL)
	{ //a child form was selected
		MessageBox( theWorkspace.LoadResourceString(IDS_CANNOTREMOVETABPANE),
								theWorkspace.LoadResourceString(IDR_MAINFRAME),
								(MB_OK | MB_ICONEXCLAMATION) );
		return;
	}

	// if it's not valid exit here
	if (hDclForm == NULL)
		return;

	if (pProjTree->GetAutoLispFileTreeItem() == hDclForm ||
			pProjTree->GetAutoLispFileParentTreeItem() == hDclForm ||
			pProjTree->GetConfigParentTreeItem() == hDclForm ||
			pProjTree->GetDockableParentTreeItem() == hDclForm ||
			pProjTree->GetModalParentTreeItem() == hDclForm ||
			pProjTree->GetModelessParentTreeItem() == hDclForm ||
			pProjTree->GetPasswordTreeItem() == hDclForm ||
			pProjTree->GetPasswordParentTreeItem() == hDclForm ||
			pProjTree->GetAxFilesParentTreeItem() == hDclForm)
	{ //the selected tree item was not a form
		MessageBox( theWorkspace.LoadResourceString(IDS_DCLTREEITEMNOTSEL),
								theWorkspace.LoadResourceString(IDR_MAINFRAME),
								(MB_OK | MB_ICONEXCLAMATION) );
		return;
	}
	
	TDclFormPtr pDclForm = pProject->FindDclForm( pProjTree->GetItemText( hDclForm ) );
	if( !pDclForm )
	{ //the form was not found for some reason
		MessageBox( theWorkspace.LoadResourceString(IDS_DCLTREEITEMNOTSEL),
								theWorkspace.LoadResourceString(IDR_MAINFRAME),
								(MB_OK | MB_ICONEXCLAMATION) );
		return;
	}

	//prompt the user to make sure they want to delete the form
	if( MessageBox( theWorkspace.LoadResourceString(IDS_QDELETEFORM),
									theWorkspace.LoadResourceString(IDR_MAINFRAME),
									(MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) ) != IDYES )
		return;

	ASSERT(pDclForm->m_htiTreeItem != NULL);
	if( pDclForm->m_htiTreeItem ) // if the tab has a tree item (which it should)
		pProjTree->DeleteItem( pDclForm->m_htiTreeItem ); // delete the item.
	pDclForm->GetProject()->DeleteForm( pDclForm ); //pDclForm is invalid after this call!

	CDocument* pDoc = pProjTree->GetDocument();
	if( pDoc )
		pDoc->SetModifiedFlag();

	pProjTree->CleanupParents();
}

void CMainFrame::OnSetautolispfilename() 
{
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	if (!pProjTree)
		return;
	TEditorProjectPtr pProject = pProjTree->GetProject();
	ASSERT(pProject != NULL);
	if (!pProject)
		return;

	CString sLispFileName = pProject->GetLispFileName();
	CFileDialog Dlg( FALSE,
									 _T(".lsp"), 
									 sLispFileName, 
									 OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
									 theWorkspace.LoadResourceString(IDS_AUTOLISPFILE),
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

void CMainFrame::OnSetpassword() 
{
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	if (!pProjTree)
		return;
	TEditorProjectPtr pProject = pProjTree->GetProject();
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

void CMainFrame::OnProjectsAddfiledialogbox() 
{
	CProjectTreeCtrl* pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
	ASSERT(pProjTree != NULL);
	if (!pProjTree)
		return;
	COpenDCLApp* pApp = (COpenDCLApp*)AfxGetApp();
	COpenDCLView *pView = pApp->AddDclFormAndView(VdclFileDialog);
	CDocument* pDoc = pProjTree->GetDocument();
	if( pDoc )
		pDoc->SetModifiedFlag();
}

void CMainFrame::OnDockProject() 
{
	ShowControlBar(&m_wndProjectTreeBar, !m_wndProjectTreeBar.IsVisible(), FALSE);		
}

void CMainFrame::OnUpdateDockProject(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_wndProjectTreeBar.IsWindowVisible());	
}

void CMainFrame::OnDockProperties() 
{
	ShowControlBar(&m_wndPropertyBar, !m_wndPropertyBar.IsVisible(), FALSE);	
}

void CMainFrame::OnUpdateDockProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_wndPropertyBar.IsWindowVisible());	
}

void CMainFrame::OnDockToolbox() 
{
	ShowControlBar(&m_wndToolBoxBar, !m_wndToolBoxBar.IsVisible(), FALSE);
}

void CMainFrame::OnUpdateDockToolbox(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_wndToolBoxBar.IsWindowVisible());	
}

void CMainFrame::OnDockZorder() 
{
	ShowControlBar(&m_wndZOderBar, !m_wndZOderBar.IsVisible(), FALSE);
}

void CMainFrame::OnUpdateDockZorder(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_wndZOderBar.IsWindowVisible());
}

void CMainFrame::OnUpdateAddmodal(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (activeProject != NULL) );
}

void CMainFrame::OnUpdateAddmodeless(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (activeProject != NULL) );
}

void CMainFrame::OnUpdateAdddockable(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (activeProject != NULL) );
}

void CMainFrame::OnUpdateAddconfig(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (activeProject != NULL) );
}

void CMainFrame::OnUpdatePicturefolder(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (activeProject != NULL) );
}
