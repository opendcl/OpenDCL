// Editor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Editor.h"
#include "ChildFrm.h"
#include "Project.h"
#include "DialogControl.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "PropertyObject.h"
#include "ProjectCollection.h"
#include "ArchiveEx.h"
#include "FontPropPage.h"
#include "GridSpacingDlg.h"
#include "ObjectBrowser.h"
#include "EditorWorkspace.h"
#include "DclControlprop.h"
#include "SharedRes.h"


static CString FindTabCaption(CDclFormObject *pDclTab)
{
	for (int i=0; i < pDclTab->GetProject()->GetDclFormList().GetCount(); i++)
	{
		POSITION pos = pDclTab->GetProject()->GetDclFormList().FindIndex(i);
		if (pos != NULL)
		{
			CDclFormObject *pDcl = pDclTab->GetProject()->GetDclFormList().GetAt(pos);
			if (pDcl != NULL)
			{
				if (pDclTab->GetParentName() == pDcl->GetUniqueName())
				{
					CDclControlObject* pControl = pDcl->FindFirstControlOfType(CtlTabStrip);
					if (!pControl)
						return CString();
					return pControl->GetPropertyListItem(Prop::TabsCaption, pDclTab->GetTabIndex());
				}
			}			
		}
	}
	return CString();
}


/////////////////////////////////////////////////////////////////////////////
// CTCOptions

CTCOptions::CTCOptions() :
   m_iLogType( TCLOG_OUTPUTWINDOW ),
   m_tUserMode( FALSE ),
   m_tAllowWindowless( TRUE ),
   m_tHonorIgnoreActivateWhenVisible( TRUE ),
   m_tIOleInPlaceSiteEx( TRUE ),
   m_tIOleInPlaceSiteWindowless( TRUE ),
   m_tIAdviseSinkEx( TRUE ),
   m_tSBindHost( TRUE )
{
}
/////////////////////////////////////////////////////////////////////////////
// COpenDCLApp

BEGIN_MESSAGE_MAP(COpenDCLApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_TOOLS_DEFAULTFONT, OnToolsDefaultfont)
	ON_COMMAND(ID_TOOLS_GRIDSPACING, OnToolsGridspacing)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EVENTSCOPYTOCLIPBOARD, OnUpdateToolsEventscopytoclipboard)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EVENTSWRITETOLISPFILE, OnUpdateToolsEventswritetolispfile)
	ON_COMMAND(ID_TOOLS_EVENTSCOPYTOCLIPBOARD, OnToolsEventscopytoclipboard)
	ON_COMMAND(ID_TOOLS_EVENTSWRITETOLISPFILE, OnToolsEventswritetolispfile)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
	// MRU - most recently used file menu
#ifndef WIN32
	ON_COMMAND_EX(ID_FILE_MRU_FILE1, OnOpenRecentFile)
	ON_COMMAND_EX(ID_FILE_MRU_FILE2, OnOpenRecentFile)
	ON_COMMAND_EX(ID_FILE_MRU_FILE3, OnOpenRecentFile)
	ON_COMMAND_EX(ID_FILE_MRU_FILE4, OnOpenRecentFile)
	ON_COMMAND_EX(ID_FILE_MRU_FILE5, OnOpenRecentFile)
	ON_COMMAND_EX(ID_FILE_MRU_FILE6, OnOpenRecentFile)
	ON_COMMAND_EX(ID_FILE_MRU_FILE7, OnOpenRecentFile)
	ON_COMMAND_EX(ID_FILE_MRU_FILE8, OnOpenRecentFile)
#else
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenDCLApp construction

COpenDCLApp::COpenDCLApp()
: m_pMainFrame( NULL )
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COpenDCLApp object

COpenDCLApp theApp;
//CMyOccManager theManager;

/////////////////////////////////////////////////////////////////////////////
// COpenDCLApp initialization

BOOL COpenDCLApp::InitInstance()
{
	//AfxEnableMemoryTracking(TRUE);
	// Initialize OLE libraries
	if( !AfxOleInit() )
	{
		return( FALSE );
	}

	AfxEnableControlContainer();//&theManager);
	EnableHtmlHelp();
	delete [] m_pszHelpFilePath;
	m_pszHelpFilePath = new TCHAR[MAX_PATH];
	lstrcpyn( (LPTSTR)m_pszHelpFilePath, theWorkspace.FindFile( _T("OpenDCL.chm") ), MAX_PATH );
	m_pCtrlHelp = NULL;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	AfxInitRichEdit();
   
	// Change the registry key under which our settings are stored.
	SetRegistryKey(theWorkspace.LoadResourceString(IDR_MAINFRAME));

	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	AddDocTemplate(
		new CMultiDocTemplate(
			IDR_ODCDOCTEMPLATE,
			RUNTIME_CLASS(COpenDCLDoc),
			RUNTIME_CLASS(CChildFrame), // custom MDI child frame
			RUNTIME_CLASS(COpenDCLView)));
	
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	m_pMainFrame = pMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	//theEditorWorkspace.SetMainFrame(pMainFrame); //save the main frame in the workspace class

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	
//	CGettingStarted Dlg(NULL, "GettingStarted.rtf");
//	Dlg.DoModal();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void COpenDCLApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwMajor;
	DWORD dwMinor;
	DWORD dwThird;
	DWORD dwFourth;
	theWorkspace.GetModuleVersionInfo( dwMajor, dwMinor, dwThird, dwFourth );
	CString sAppVersion;
	sAppVersion.Format( _T("%d.%d.%d.%d"), dwMajor, dwMinor, dwThird, dwFourth );
	SetDlgItemText( IDC_APPVERSION, sAppVersion );

	CString sLicenseTxt;
#ifdef _DEBUG
	//in the development environment, License.txt is not in the app folder, so search for it
	sLicenseTxt = theWorkspace.FindFile( _T("License.txt") );
#else
	GetModuleFileName( NULL, sLicenseTxt.GetBuffer( MAX_PATH ), MAX_PATH );
	sLicenseTxt.ReleaseBuffer();
	sLicenseTxt = sLicenseTxt.Mid( sLicenseTxt.MakeReverse().SpanExcluding( _T("\\/:") ).GetLength() );
	sLicenseTxt.MakeReverse() += _T("License.txt");
#endif //_DEBUG
	try
	{
		CStdioFile file( sLicenseTxt, CFile::modeRead | CFile::shareDenyNone );
		UINT cbText = (UINT)file.GetLength();
		CStringA sText;
		cbText = file.Read( sText.GetBuffer( cbText ), cbText );
		sText.ReleaseBuffer( cbText );
		CRichEditCtrl* pTextBox = (CRichEditCtrl*)GetDlgItem(IDC_LICENSETXT);
		::SetWindowTextA( ::GetDlgItem( m_hWnd, IDC_LICENSETXT ), sText );
		pTextBox->SetOptions(ECOOP_OR, ECO_SAVESEL);
		pTextBox->SetSel(0, 0);
	}
	catch( CFileException* e )
	{
		e->ReportError();
		e->Delete();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// COpenDCLApp message handlers

void COpenDCLApp::OnFileNew() 
{
	if( !SaveAllModified() )
		return;
	CloseAllDocuments(FALSE);
	CWinApp::OnFileNew();
}

void COpenDCLApp::OnFileOpen() 
{
	if( !SaveAllModified() )
		return;
	CloseAllDocuments(FALSE);
	CWinApp::OnFileOpen();
}

BOOL COpenDCLApp::OnOpenRecentFile(UINT nID)
{
	if( !SaveAllModified() )
		return FALSE;
	CloseAllDocuments(FALSE);
	return CWinApp::OnOpenRecentFile(nID);
}

CMDIChildWnd* COpenDCLApp::CreateOrActivateFrame(CDocument* pDoc, CSize ViewSize, bool bResizable)
{
	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	CDocTemplate* pDocTemplate = m_pDocManager->GetNextDocTemplate(pos);
	ASSERT(pDocTemplate != NULL);
	CFrameWnd* pNewFrame = pDocTemplate->CreateNewFrame(pDoc, NULL);
	if (pNewFrame == NULL) return NULL;  // not created
	
	// set the startup size	
	((CChildFrame*)pNewFrame)->m_StartupSize.cx = ViewSize.cx;
	((CChildFrame*)pNewFrame)->m_StartupSize.cy = ViewSize.cy;
	if (!bResizable)
		pNewFrame->ModifyStyle(WS_THICKFRAME, DS_MODALFRAME, 0);
	pNewFrame->ModifyStyleEx(WS_EX_CLIENTEDGE, NULL, 0);// remove the client edge style
	pDocTemplate->InitialUpdateFrame(pNewFrame, pDoc);
  return (CMDIChildWnd*)pNewFrame;
}

COpenDCLView* COpenDCLApp::OpenExistingForm(CDclFormObject *pDclForm)
{
	CDclControlObject* pDclProperties = pDclForm->GetControlProperties();
	CSize StartupSize(pDclProperties->GetLongProperty(Prop::Width), pDclProperties->GetLongProperty(Prop::Height));
	
	// create the new view
	bool bResizable = true;
	if (pDclForm->GetType() == VdclConfigTab || pDclForm->GetType() == VdclTabForm)
		bResizable = false;
	
	CMDIChildWnd* pNewFrame = CreateOrActivateFrame(theWorkspace.GetActiveDocument()/*m_pMainFrame->GetActiveDocument()*/, StartupSize, bResizable);
	COpenDCLView* pNewView = (COpenDCLView*)pNewFrame->GetActiveView();

	// set the dcl pointers
	pNewView->m_pThisDclForm = pDclForm;
	pDclForm->m_pChildWnd = pNewView;
	pDclForm->m_pMdiChildWnd = pNewFrame;

	ASSERT(pNewView != NULL);
	ASSERT(pNewView->IsKindOf(RUNTIME_CLASS(COpenDCLView)));

	if (pDclForm->UsesClientRect())
	{
		CRect rcNewDialog;
		pNewFrame->GetWindowRect(rcNewDialog);
		CRect rcView;
		pNewView->GetClientRect(&rcView);
		pNewFrame->SetWindowPos(NULL, -1, -1,
														rcNewDialog.Width() * 2 - rcView.Width(),
														rcNewDialog.Height() * 2 - rcView.Height(),
														SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	if (pDclForm != NULL)
	{
		if (pDclForm->GetType() == VdclFileDialog)
			((CChildFrame*)pNewFrame)->SetTitleBarIcon(-1);
		else
			((CChildFrame*)pNewFrame)->SetTitleBarIcon(pDclForm->GetTitleBarIcon());
		if (pDclForm->GetType() != VdclTabForm)
			((CChildFrame*)pNewFrame)->m_Title = pDclForm->GetTitleText();
		else
			((CChildFrame*)pNewFrame)->m_Title = FindTabCaption(pDclForm);
		pNewFrame->SetWindowText(((CChildFrame*)pNewFrame)->m_Title);
	}
    
	// call the method to display the controls to the user
	pNewView->DisplayControls(pDclForm);
	if( !pNewView->m_SelectedControl.GetTemplate() )
		pNewView->m_SelectedControl.Set( pDclProperties );
	theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties( pDclProperties, pNewView );
	return pNewView;
}

COpenDCLView* COpenDCLApp::AddDclFormAndView(DclFormType nType) 
{
	CRect rcNewDialog;
	CMDIChildWnd* pNewFrame;

	CDclFormObject* pNewDcl = AddNewDclForm(nType);
	CDclControlObject *pDclProperties = pNewDcl->GetControlProperties();
	assert(pDclProperties != NULL);

	// get the arx object that holds the dialog's properties
	CSize StartupSize(pDclProperties->GetLongProperty(Prop::Width), pDclProperties->GetLongProperty(Prop::Height));
	
	// create the new view
	CDocument* pDoc = theEditorWorkspace.GetActiveDocument();
	if (!pDoc)
		return NULL;

	bool bResizable = true;
	if (nType == VdclTabForm)
		bResizable = false;
	pNewFrame = CreateOrActivateFrame(pDoc, StartupSize, bResizable);
	
	if (!pNewFrame)
		return NULL;

	if (pNewDcl != NULL)
	{
		if (pNewDcl->GetType() == VdclFileDialog)
			((CChildFrame*)pNewFrame)->SetTitleBarIcon(-1); // set the title bar icon.
	}

	COpenDCLView* pNewView = (COpenDCLView*)pNewFrame->GetActiveView();

	// set the dcl pointers
	pNewDcl->m_pChildWnd = pNewView;
	pNewDcl->m_pMdiChildWnd = pNewFrame;

	ASSERT(pNewView != NULL);
	ASSERT(pNewView->IsKindOf(RUNTIME_CLASS(COpenDCLView)));

	// get the position of the new dialog box
	pNewFrame->GetWindowRect(rcNewDialog);
	pNewFrame->GetParent()->ScreenToClient(rcNewDialog);
	
	// reset the size of the new dialog box
	rcNewDialog.right = rcNewDialog.left + pDclProperties->GetLongProperty(Prop::Width);
	rcNewDialog.bottom = rcNewDialog.top + pDclProperties->GetLongProperty(Prop::Height);

	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
	pZOrderList->ClearSelection();

	if (pNewDcl != NULL)
		pNewFrame->SetWindowText(pNewDcl->GetTitleText());
	pNewView->m_pThisDclForm = pNewDcl;
	if (pNewDcl->GetType() == VdclFileDialog && pNewDcl->GetControlCount() == 1)
	{
		CRect rc(0, 0, 556, 386); //size of IDD_FILEDLGCTRL bitmap
		CDclControlObject* pDclControl = pNewView->InsertControl(rc, CtlFileDlgCtrl);
		if( pDclControl )
		{
			CRect rcCtrl;
			pDclControl->GetControlInstance()->GetControl()->GetWindowRect( &rcCtrl );
			CRect rcClient;
			pNewFrame->GetClientRect( &rcClient );
			pNewFrame->GetWindowRect( &rc );
			pNewFrame->GetParent()->ScreenToClient( &rc );
			int nNewWidth = rc.Width() - rcClient.Width() + rcCtrl.Width();
			int nNewHeight = rc.Height() - rcClient.Height() + rcCtrl.Height();
			pNewDcl->GetControlProperties()->SetLongProperty( Prop::Width, nNewWidth );
			pNewDcl->GetControlProperties()->SetLongProperty( Prop::Height, nNewHeight );
			pNewFrame->MoveWindow( rc.left, rc.top, nNewWidth, nNewHeight );
		}
	}
	else
	{
		// call the method to display the controls to the user
		pNewView->DisplayControls(pNewDcl);
	}
	if( !pNewView->m_SelectedControl.GetTemplate() )
		pNewView->m_SelectedControl.Set( pDclProperties );
	theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties( pDclProperties, pNewView );

	if (pNewDcl->m_htiTreeItem != NULL)
	{
		CProjectTreeCtrl *pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
		pProjTree->SelectItem(pNewDcl->m_htiTreeItem);
	}
	return pNewView;
}

CDclFormObject* COpenDCLApp::AddNewDclForm(DclFormType nType) 
{
	// create a pointer to pass to the list to insert
	CDclFormObject* pNewDclForm = activeProject->AddForm( nType );
	if( pNewDclForm )
	{	
		AddDefaultProperties(pNewDclForm->GetControlProperties(), -1, -1); //add properties to the new dcl form object
		CProjectTreeCtrl *pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
		if( pProjTree )
			pProjTree->AddFormToTree(pNewDclForm, true); //add the new dcl form to the project tree
		theWorkspace.SetModified(true);
	}
	return pNewDclForm;
}

void COpenDCLApp::OnToolsDefaultfont() 
{
	CPropertySheet Dlg;
	CFontPropertyPage FontPage;
			
	Dlg.AddPage(&FontPage);
	
	// set the title
	CString sTitle;
	sTitle = theWorkspace.LoadResourceString(IDS_DEFFONT);
	Dlg.SetTitle(sTitle);

	// call the dialog box
	Dlg.DoModal();
}

int COpenDCLApp::ExitInstance() 
{
	if (m_pCtrlHelp != NULL)
		delete m_pCtrlHelp;
	return CWinApp::ExitInstance();
}

void COpenDCLApp::OnToolsGridspacing() 
{
	CGridSpacingDlg Dlg;
	Dlg.DoModal();
}

void COpenDCLApp::OnUpdateToolsEventscopytoclipboard(CCmdUI* pCmdUI) 
{
	BOOL bChecked =
		AfxGetApp()->GetProfileInt(theWorkspace.LoadResourceString(IDR_MAINFRAME), _T("EventsCopyToClipboard"), TRUE);
	pCmdUI->SetCheck(bChecked);	
}

void COpenDCLApp::OnUpdateToolsEventswritetolispfile(CCmdUI* pCmdUI) 
{
	BOOL bChecked =
		AfxGetApp()->GetProfileInt(theWorkspace.LoadResourceString(IDR_MAINFRAME), _T("EventsWriteToLispFile"), FALSE);
	pCmdUI->SetCheck(bChecked);	
}

void COpenDCLApp::OnToolsEventscopytoclipboard() 
{
	CString sSection = theWorkspace.LoadResourceString(IDR_MAINFRAME);
	BOOL bNewVal = !AfxGetApp()->GetProfileInt(sSection, _T("EventsCopyToClipboard"), TRUE);
	AfxGetApp()->WriteProfileInt(sSection, _T("EventsCopyToClipboard"), bNewVal);
	theEditorWorkspace.GetPropertyTabs()->m_EventsTabPane.GetDlgItem(IDC_COPYTOCLIPBOARD)->ShowWindow(bNewVal? SW_SHOW : SW_HIDE);	
}

void COpenDCLApp::OnToolsEventswritetolispfile() 
{
	CString sSection = theWorkspace.LoadResourceString(IDR_MAINFRAME);
	BOOL bNewVal = !AfxGetApp()->GetProfileInt(sSection, _T("EventsWriteToLispFile"), FALSE);
	AfxGetApp()->WriteProfileInt(sSection, _T("EventsWriteToLispFile"), bNewVal);
	theEditorWorkspace.GetPropertyTabs()->m_EventsTabPane.GetDlgItem(IDC_ADDTOLISP)->ShowWindow(bNewVal? SW_SHOW : SW_HIDE);	
}

void COpenDCLApp::OnHelp() 
{
	HtmlHelp(0, HH_DISPLAY_TOPIC);
}

void COpenDCLApp::OnHelpFinder() 
{
}
