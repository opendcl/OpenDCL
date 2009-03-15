// StudioFrame.cpp : implementation of the CStudioFrame class
//

#include "stdafx.h"
#include "OpenDCL.h"
#include "StudioFrame.h"
#include "OpenDCLDoc.h"
#include "DclFormView.h"
#include "DclFormFrame.h"
#include "DclControlProp.h"
#include "ControlName.h"
#include "StudioWorkspace.h"
#include "StudioDialogControl.h"
#include "StudioDialogObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CStudioFrame

IMPLEMENT_DYNCREATE(CStudioFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CStudioFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_UNDO, &CStudioFrame::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CStudioFrame::OnUpdateEditUndo)
	ON_COMMAND(ID_WINDOW_CLOSE, &CStudioFrame::OnWindowClose)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CLOSE, &CStudioFrame::OnUpdateWindowClose)
	ON_COMMAND(ID_WINDOW_CLOSEALL, &CStudioFrame::OnWindowCloseAll)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CLOSEALL, &CStudioFrame::OnUpdateWindowCloseAll)
	ON_COMMAND(ID_VIEW_FONTTOOLBAR, &CStudioFrame::OnViewFontToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FONTTOOLBAR, &CStudioFrame::OnUpdateViewFontToolbar)
	ON_COMMAND(ID_VIEW_PROJECT, &CStudioFrame::OnViewProject)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT, &CStudioFrame::OnUpdateViewProject)
	ON_COMMAND(ID_VIEW_PROPERTIES, &CStudioFrame::OnViewProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, &CStudioFrame::OnUpdateViewProperties)
	ON_COMMAND(ID_VIEW_TOOLBOX, &CStudioFrame::OnViewToolbox)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBOX, &CStudioFrame::OnUpdateViewToolbox)
	ON_COMMAND(ID_VIEW_ZORDER, &CStudioFrame::OnViewZorder)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZORDER, &CStudioFrame::OnUpdateViewZorder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CStudioFrame construction/destruction

CStudioFrame::CStudioFrame()
: m_wndFontToolBar( this )
, m_pwndProjectPane( new CProjectPane )
{
	theStudioWorkspace.SetStudioFrame( this );
}

CStudioFrame::~CStudioFrame()
{
	theStudioWorkspace.SetStudioFrame( NULL );
}

// This function is Copyright (c) 2000, Cristi Posea.
// See www.datamekanix.com for more control bars tips&tricks.
BOOL CStudioFrame::VerifyBarState( LPCTSTR lpszProfileName )
{
	CDockState state;
	state.LoadState( lpszProfileName );

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

void CStudioFrame::OnFontChange( const FontSettings& FS, UINT flags /*= fontAll*/ )
{
	if( !FS )
		return;
	CStudioDialogObject* pDlgObject = theStudioWorkspace.GetActiveDlgObject();
	if( pDlgObject )
		pDlgObject->OnFontChange( FS, flags );
	else
		theApp.SetDefaultFontSettings( FS, flags );
}

TDclFormPtr CStudioFrame::AddNewDclFormView( FormType nType )
{
	TStudioProjectPtr pProject = theStudioWorkspace.GetActiveProject();
	if( !pProject )
		return NULL;

	UINT nFormId = 1;
	CString sFormName;
	bool bFoundUnusedName = false;
	CString sFormBase = GetControlSimpleName( _CtlForm );
	while( !bFoundUnusedName )
	{
		sFormName.Format( _T("%s%d"), (LPCTSTR)sFormBase, nFormId++ );
		bFoundUnusedName = !pProject->FindDclForm( sFormName );
	}

	TDclFormPtr pNewDclForm = pProject->AddForm( nType, sFormName );
	if( !pNewDclForm )
		return NULL;
	UINT nGridSpacing = theApp.GetGridSpacing();
	if( nGridSpacing > 1 )
	{
		TDclControlPtr pFormProps = pNewDclForm->GetControlProperties();
		LONG X = pFormProps->GetLongProperty( Prop::Width ) + (nGridSpacing / 2);
		LONG Y = pFormProps->GetLongProperty( Prop::Height ) + (nGridSpacing / 2);
		pFormProps->SetLongProperty( Prop::Width, X - (X % nGridSpacing) );
		pFormProps->SetLongProperty( Prop::Height, Y - (Y % nGridSpacing) );
	}
	CProjectPane* pProjTree = theStudioWorkspace.GetProjectPane();
	if( pProjTree )
		pProjTree->AddFormToTree( pNewDclForm, true ); //add the new dcl form to the project tree
	OpenDclFormView( pNewDclForm );
	return pNewDclForm;
}

CDclFormView* CStudioFrame::OpenDclFormView( TDclFormPtr pDclForm )
{
	CDclFormView* pFormView = NULL;
	CDialogObject* pDlgObject = pDclForm->GetFormInstance();
	if( pDlgObject )
		pFormView = ((CStudioDialogObject*)pDlgObject)->GetFormView();
	else
	{
		TDclControlPtr pDclProperties = pDclForm->GetControlProperties();
		CSize sizeStartup(pDclProperties->GetLongProperty(Prop::Width), pDclProperties->GetLongProperty(Prop::Height));

		CCreateContext ctxtFormView;
		ctxtFormView.m_pNewViewClass = RUNTIME_CLASS(CDclFormView);
		ctxtFormView.m_pCurrentDoc = pDclForm->GetProject()->GetDocument();
		ctxtFormView.m_pNewDocTemplate = NULL;
		ctxtFormView.m_pLastView = NULL;
		ctxtFormView.m_pCurrentFrame = this;
		CDclFormFrame* pDclFormFrame = (CDclFormFrame*)theStudioWorkspace.GetStudioFrame()->CreateNewChild( RUNTIME_CLASS(CDclFormFrame), IDD_FORMVIEWFRAME );
		if( !theStudioWorkspace.GetActiveFormView() )
			pDclFormFrame->MDIMaximize();
		pFormView = (CDclFormView*)pDclFormFrame->CreateView( &ctxtFormView );
		if( !pFormView )
			return NULL;

		pFormView->SetScaleToFitSize( CSize( 1200, 800 ) );
		pFormView->SetSourceForm( pDclForm );
		pDclFormFrame->SetDialogObject( pFormView->GetDialogObject() );
		pFormView->SendMessage( WM_INITIALUPDATE, 0, 0 );
		RecalcLayout();
		pFormView->UpdateWindow();
	}
	theStudioWorkspace.ActivateDlgObject( pFormView->GetDialogObject() );
	pFormView->GetParentFrame()->ActivateFrame();
	return pFormView;
}

int CStudioFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CWinApp* pApp = AfxGetApp();
	WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
	GetWindowPlacement( &wp );
	CRect& rcWnd = (CRect&)wp.rcNormalPosition;
	wp.showCmd = pApp->GetProfileInt( theWorkspace.GetAppKey(), _T("State"), wp.showCmd );
	rcWnd.left = pApp->GetProfileInt( theWorkspace.GetAppKey(), _T("TopLeftX"), rcWnd.left );
	rcWnd.top = pApp->GetProfileInt( theWorkspace.GetAppKey(), _T("TopLeftY"), rcWnd.top );
	rcWnd.right = rcWnd.left + pApp->GetProfileInt( theWorkspace.GetAppKey(), _T("Width"), rcWnd.Width() );
	rcWnd.bottom = rcWnd.top + pApp->GetProfileInt( theWorkspace.GetAppKey(), _T("Height"), rcWnd.Height() );
	SetWindowPlacement( &wp );
	if( wp.showCmd == SW_MAXIMIZE )
		PostMessage( WM_SYSCOMMAND, SC_MAXIMIZE, 0 );

	if (!m_wndStatusBar.Create(this) ||
			!m_wndStatusBar.SetIndicators(indicators,  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
	if (!m_wndMainToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
			| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
			!m_wndMainToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create main toolbar\n");
		return -1;      // fail to create
	}

	// setup the "Add Form" drop down button in the main toolbar
	m_wndMainToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	TBBUTTONINFO tbi = { sizeof(TBBUTTONINFO), TBIF_STYLE };
	m_wndMainToolBar.GetToolBarCtrl().GetButtonInfo(ID_ADDMODAL, &tbi);
	tbi.fsStyle |= TBSTYLE_DROPDOWN;
	m_wndMainToolBar.GetToolBarCtrl().SetButtonInfo(ID_ADDMODAL, &tbi);
	m_wndMainToolBar.GetToolBarCtrl().GetButtonInfo(ID_EDIT_UNDO, &tbi);
	tbi.fsStyle |= TBSTYLE_DROPDOWN;
	m_wndMainToolBar.GetToolBarCtrl().SetButtonInfo(ID_EDIT_UNDO, &tbi);
	m_wndMainToolBar.DrawMenuBar();

	if (!m_wndFontToolBar.Create(this, IDR_FONTBTNS, CBRS_ALIGN_TOP, ID_VIEW_FONTTOOLBAR))
	{
		TRACE0("Failed to create font dialogbar\n");
		return -1;		// fail to create
	}
	
	if (!m_wndReBar.Create(this) ||
			!m_wndReBar.AddBar(&m_wndMainToolBar) ||
			!m_wndReBar.AddBar(&m_wndFontToolBar))		
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_TOP | CBRS_ALIGN_RIGHT);
	m_wndMainToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndFontToolBar.EnableDocking(CBRS_ALIGN_ANY);

#ifdef _SCB_REPLACE_MINIFRAME
    m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
#endif //_SCB_REPLACE_MINIFRAME

	// create toolbox sizing bar and pane
	if (!m_wndToolBoxSizingBar.Create(_T("Instant Bar"), this, 127))
	{
		TRACE0("Failed to create toolbox instant bar\n");
		return -1;		// fail to create
	}
	m_wndToolBoxSizingBar.SetWindowText(theWorkspace.LoadResourceString(IDS_TOOLBOX));
	m_wndToolBoxSizingBar.SetSCBStyle(m_wndToolBoxSizingBar.GetSCBStyle() | SCBS_SIZECHILD);
	m_wndToolBoxSizingBar.SetBarStyle(m_wndToolBoxSizingBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndToolBoxSizingBar.SetBarStyle(m_wndToolBoxSizingBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBoxSizingBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	if (!m_wndToolBoxPane.Create(IDD_OPENDCL_PANE, &m_wndToolBoxSizingBar))
	{
		TRACE0("Failed to create toolbox pane\n");
		return -1;		// fail to create
	}
	m_wndToolBoxPane.ShowWindow(SW_SHOW);
	RecalcLayout();

	// create project sizing bar and pane
	if (!m_wndProjectSizingBar.Create(_T("Instant Bar"), this, 128))
	{
		TRACE0("Failed to create project instant bar\n");
		return -1;		// fail to create
	}
	m_wndProjectSizingBar.SetWindowText(theWorkspace.LoadResourceString(IDS_PROJECT));
	m_wndProjectSizingBar.SetSCBStyle(m_wndProjectSizingBar.GetSCBStyle() | SCBS_SIZECHILD);
	m_wndProjectSizingBar.SetBarStyle(m_wndProjectSizingBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndProjectSizingBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	CCreateContext ctxtProjectView;
	ctxtProjectView.m_pNewViewClass = RUNTIME_CLASS(CProjectPane);
	ctxtProjectView.m_pCurrentDoc = GetActiveDocument();
	ctxtProjectView.m_pNewDocTemplate = NULL;
	ctxtProjectView.m_pLastView = NULL;
	ctxtProjectView.m_pCurrentFrame = NULL;
	if (!m_pwndProjectPane->Create( NULL, NULL,
																	WS_CHILD|WS_VISIBLE|TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT|TVS_SHOWSELALWAYS,
																	CRect(0, 0, 0, 0), &m_wndProjectSizingBar, 100, &ctxtProjectView ))
	{
		TRACE0("Failed to create project pane\n");
		return -1;		// fail to create
	}
	m_pwndProjectPane->ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	CWnd* pTT = FromHandle((HWND) m_pwndProjectPane->SendMessage(TVM_GETTOOLTIPS));
	if (pTT != NULL)
		pTT->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	m_pwndProjectPane->OnActivateProject(NULL);
	SetActiveView( m_pwndProjectPane );

	// create property sizing bar and pane
	if (!m_wndPropertySizingBar.Create(_T("Instant Bar"), this, 130 ))
	{
		TRACE0("Failed to create property instant bar\n");
		return -1;		// fail to create
	}
	m_wndPropertySizingBar.SetWindowText(theWorkspace.LoadResourceString(IDS_PROPERTIES));
	m_wndPropertySizingBar.SetSCBStyle(m_wndPropertySizingBar.GetSCBStyle() | SCBS_SIZECHILD);
	m_wndPropertySizingBar.SetBarStyle(m_wndPropertySizingBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndPropertySizingBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	if (!m_wndPropertyPane.Create(IDD_OPENDCL_PANE, &m_wndPropertySizingBar))
	{
		TRACE0("Failed to create property pane\n");
		return -1;		// fail to create
	}
	m_wndPropertyPane.ShowWindow(SW_SHOW);

	// create Z order sizing bar and pane
	if (!m_wndZOrderSizingBar.Create(_T("Instant Bar"), this, 131 ))
	{
		TRACE0("Failed to create Z order instant bar\n");
		return -1;		// fail to create
	}
	m_wndZOrderSizingBar.SetWindowText(theWorkspace.LoadResourceString(IDS_ZTABORDER));
	m_wndZOrderSizingBar.SetSCBStyle(m_wndZOrderSizingBar.GetSCBStyle() | SCBS_SIZECHILD);
	m_wndZOrderSizingBar.SetBarStyle(m_wndZOrderSizingBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndZOrderSizingBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	if (!m_wndZOrderPane.Create(IDD_OPENDCL_PANE, &m_wndZOrderSizingBar))
	{
		TRACE0("Failed to create Z order pane\n");
		return -1;		// fail to create
	}
	m_wndZOrderPane.ShowWindow(SW_SHOW);

	DockControlBar(&m_wndToolBoxSizingBar, AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndPropertySizingBar, AFX_IDW_DOCKBAR_RIGHT);

	CRect rBar;
	m_wndToolBoxSizingBar.GetWindowRect(rBar);	
	rBar.OffsetRect(0, 1);
	DockControlBar(&m_wndProjectSizingBar, AFX_IDW_DOCKBAR_LEFT, rBar);
	RecalcLayout();

	m_wndPropertySizingBar.GetWindowRect(rBar);
	rBar.OffsetRect(1, 1);
	DockControlBar(&m_wndZOrderSizingBar, AFX_IDW_DOCKBAR_RIGHT, rBar);
	RecalcLayout();

	static LPCTSTR pszBarState = _T("BarState");
	if( VerifyBarState( pszBarState ) )
	{
		CSizingControlBar::GlobalLoadState( this, pszBarState );
		LoadBarState( pszBarState );
	}

	return 0;
}

BOOL CStudioFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~(FWS_ADDTOTITLE | FWS_PREFIXTITLE); //handle main frame title in OnActivateProject
	return __super::PreCreateWindow(cs);
}


// CStudioFrame diagnostics

#ifdef _DEBUG
void CStudioFrame::AssertValid() const
{
	__super::AssertValid();
}

void CStudioFrame::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG


// CStudioFrame message handlers


BOOL CStudioFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	return __super::OnCreateClient(lpcs, pContext);
}

void CStudioFrame::OnDestroy() 
{
	CWinApp* pApp = AfxGetApp();
	WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
	GetWindowPlacement( &wp );
	CRect& rcWnd = (CRect&)wp.rcNormalPosition;
	pApp->WriteProfileInt( theWorkspace.GetAppKey(), _T("State"), wp.showCmd );
	pApp->WriteProfileInt( theWorkspace.GetAppKey(), _T("TopLeftX"), rcWnd.left );
	pApp->WriteProfileInt( theWorkspace.GetAppKey(), _T("TopLeftY"), rcWnd.top );
	pApp->WriteProfileInt( theWorkspace.GetAppKey(), _T("Width"), rcWnd.Width() );
	pApp->WriteProfileInt( theWorkspace.GetAppKey(), _T("Height"), rcWnd.Height() );

	static const LPCTSTR pszBarState = _T("BarState");
	CSizingControlBar::GlobalSaveState( this, pszBarState );
	SaveBarState( pszBarState );

	__super::OnDestroy();
}

BOOL CStudioFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LPNMTOOLBAR lpnmTB = (LPNMTOOLBAR)lParam;
	switch(lpnmTB->hdr.code)
	{
	case TBN_DROPDOWN:
		switch( lpnmTB->iItem )
		{
		case ID_ADDMODAL:
			OnAddFormButtonDropdown();
			return TRUE;
		case ID_EDIT_UNDO:
			OnUndoButtonDropdown();
			return TRUE;
		}
	}
	return __super::OnNotify(wParam, lParam, pResult);
}

void CStudioFrame::OnEditUndo() 
{
	TStudioProjectPtr pProject = theStudioWorkspace.GetActiveProject();
	if( pProject )
		pProject->Undo();
}

void CStudioFrame::OnUpdateEditUndo(CCmdUI *pCmdUI) 
{
	TStudioProjectPtr pProject = theStudioWorkspace.GetActiveProject();
	pCmdUI->Enable( pProject && pProject->UndoCount() > 0 );
}

void CStudioFrame::OnWindowClose()
{
	CDclFormView* pView = theStudioWorkspace.GetActiveFormView();
	if( pView )
	{
		SetActiveView( theStudioWorkspace.GetProjectPane() );
		pView->GetParentFrame()->DestroyWindow();
	}
}

void CStudioFrame::OnUpdateWindowClose(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (theStudioWorkspace.GetActiveFormView() != NULL) );
}

void CStudioFrame::OnWindowCloseAll()
{
	CDocument* pDoc = GetActiveDocument();
	if( pDoc )
	{
		CStudioFrame* pStudioFrame = theStudioWorkspace.GetStudioFrame();
		CProjectPane* pProjectPane = theStudioWorkspace.GetProjectPane();
		pStudioFrame->SetActiveView( pProjectPane );
		POSITION pos = pDoc->GetFirstViewPosition();
		while( pos )
		{
			CView* pView = pDoc->GetNextView( pos );
			if( pView != pProjectPane )
				pView->GetParentFrame()->DestroyWindow();
		}
	}
}

void CStudioFrame::OnUpdateWindowCloseAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (theStudioWorkspace.GetActiveFormView() != NULL) );
}

void CStudioFrame::OnViewFontToolbar() 
{
	ShowControlBar( &m_wndFontToolBar, !m_wndFontToolBar.IsVisible(), FALSE );		
}

void CStudioFrame::OnUpdateViewFontToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_wndFontToolBar.IsWindowVisible() );	
}

void CStudioFrame::OnViewProject() 
{
	ShowControlBar( &m_wndProjectSizingBar, !m_wndProjectSizingBar.IsVisible(), FALSE );		
}

void CStudioFrame::OnUpdateViewProject(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_wndProjectSizingBar.IsWindowVisible() );	
}

void CStudioFrame::OnViewProperties() 
{
	ShowControlBar( &m_wndPropertySizingBar, !m_wndPropertySizingBar.IsVisible(), FALSE );	
}

void CStudioFrame::OnUpdateViewProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_wndPropertySizingBar.IsWindowVisible() );	
}

void CStudioFrame::OnViewToolbox() 
{
	ShowControlBar( &m_wndToolBoxSizingBar, !m_wndToolBoxSizingBar.IsVisible(), FALSE );
}

void CStudioFrame::OnUpdateViewToolbox(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_wndToolBoxSizingBar.IsWindowVisible() );	
}

void CStudioFrame::OnViewZorder() 
{
	ShowControlBar( &m_wndZOrderSizingBar, !m_wndZOrderSizingBar.IsVisible(), FALSE );
}

void CStudioFrame::OnUpdateViewZorder(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_wndZOrderSizingBar.IsWindowVisible() );
}

void CStudioFrame::OnUndoButtonDropdown() 
{
	TStudioProjectPtr pProject = theStudioWorkspace.GetActiveProject();
	if( !pProject )
		return;
	CUndoManager* pUndoManager = pProject->GetUndoManager();
	if( !pUndoManager || !pUndoManager->enabled() )
		return;
	CStringArray rsUndoActions;
	if( !pUndoManager->GetUndoableActions( rsUndoActions, 10 ) )
		return;
	if( rsUndoActions.IsEmpty() )
		return;
	CMenu menu;
	menu.CreatePopupMenu();
	MENUITEMINFO item =
	{
		sizeof(MENUITEMINFO),
		MIIM_TYPE/* | MIIM_STRING*/ | MIIM_STATE | MIIM_ID,
		MFT_STRING,
		MFS_ENABLED,
		0,
		NULL,
		NULL,
		NULL,
		0,
		NULL,
		0,
	};
	for( INT_PTR idx = rsUndoActions.GetSize() - 1; idx >= 0; --idx )
	{
		CString sItem = rsUndoActions.GetAt( idx );
		item.wID = idx + 1;
		item.cch = sItem.GetLength();
		item.dwTypeData = sItem.LockBuffer();
		menu.InsertMenuItem( 0, &item, TRUE );
	}
	CRect rc;
	m_wndMainToolBar.GetWindowRect(&rc);
	menu.SetDefaultItem( 1 );
	UINT ctUndo = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY | TPM_LEFTALIGN | TPM_LEFTBUTTON, rc.left + 150, rc.bottom, this);
	if( ctUndo == 0 )
		return;
	((CStudioUndoManager*)pUndoManager)->Undo( ctUndo );
}

void CStudioFrame::OnAddFormButtonDropdown() 
{
	CMenu menu;
	menu.LoadMenu(IDR_POPUPMENUS);
	CMenu* pPopup = menu.GetSubMenu( 0 );
	ASSERT(pPopup != NULL);
	
	CRect rc;
	m_wndMainToolBar.GetWindowRect(&rc);
	menu.SetDefaultItem( ID_ADDMODAL );
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rc.left + 194, rc.bottom, this);
}
