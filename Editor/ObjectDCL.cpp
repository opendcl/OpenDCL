// ObjectDCL.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ObjectDCL.h"
#include "PurchaseMode.h"
#include "ChildFrm.h"
#include "Project.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "PropertyObject.h"
#include "ProjectCollection.h"
#include "ArchiveEx.h"
#include "FontPropPage.h"
#include "Help1.h"
#include "Help2.h"
#include "GridSpacingDlg.h"
#include "ObjectBrowser.h"
#include "EditorWorkspace.h"
#include "DclControlprop.h"
#include "SharedRes.h"


static CString FindTabCaption(CDclFormObject *pDclTab)
{
	// do loop to add all the tree items
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
					return pControl->GetPropertyListItem(nTabsCaption, pDclTab->GetTabIndex());
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
// CObjectDCLApp

BEGIN_MESSAGE_MAP(CObjectDCLApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_CREATEDISTFILE, OnFileCreatedistfile)
	ON_COMMAND(ID_TOOLS_DEFAULTFONT, OnToolsDefaultfont)
	ON_COMMAND(ID_TOOLS_GRIDSPACING, OnToolsGridspacing)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EVENTSCOPYTOCLIPBOARD, OnUpdateToolsEventscopytoclipboard)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EVENTSWRITETOLISPFILE, OnUpdateToolsEventswritetolispfile)
	ON_COMMAND(ID_TOOLS_EVENTSCOPYTOCLIPBOARD, OnToolsEventscopytoclipboard)
	ON_COMMAND(ID_TOOLS_EVENTSWRITETOLISPFILE, OnToolsEventswritetolispfile)
	ON_COMMAND(ID_TOOLS_EXPORTLANGUAGEEXCELSPREADSHEET, OnToolsExportlanguageexcelspreadsheet)
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
// CObjectDCLApp construction

CObjectDCLApp::CObjectDCLApp()
: m_pMainFrame( NULL )
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CObjectDCLApp object

CObjectDCLApp theApp;
//CMyOccManager theManager;

/////////////////////////////////////////////////////////////////////////////
// CObjectDCLApp initialization

BOOL CObjectDCLApp::InitInstance()
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

	CMultiDocTemplate* pDocTemplate = NULL;
	switch (nCurrentPurchaseMode)
		{
		case nPurchasedLT:
			pDocTemplate = new CMultiDocTemplate(
				IDR_OBJECTTYPE_LT,
				RUNTIME_CLASS(CObjectDCLDoc),
				RUNTIME_CLASS(CChildFrame), // custom MDI child frame
				RUNTIME_CLASS(CObjectDCLView));
			break;
		case nPurchasedR14Pro:
		case nPurchasedGMP:
			pDocTemplate = new CMultiDocTemplate(
				IDR_OBJECTTYPE_R14,
				RUNTIME_CLASS(CObjectDCLDoc),
				RUNTIME_CLASS(CChildFrame), // custom MDI child frame
				RUNTIME_CLASS(CObjectDCLView));
			break;
		case nPurchasedStd:			
			pDocTemplate = new CMultiDocTemplate(
				IDR_OBJECTTYPE_STD,
				RUNTIME_CLASS(CObjectDCLDoc),
				RUNTIME_CLASS(CChildFrame), // custom MDI child frame
				RUNTIME_CLASS(CObjectDCLView));
			break;
		default:			
			pDocTemplate = new CMultiDocTemplate(
				IDR_OBJECTTYPE,
				RUNTIME_CLASS(CObjectDCLDoc),
				RUNTIME_CLASS(CChildFrame), // custom MDI child frame
				RUNTIME_CLASS(CObjectDCLView));
			break;
		}
	AddDocTemplate(pDocTemplate);
	
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
void CObjectDCLApp::OnAppAbout()
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
		pTextBox->SetWindowTextA( sText );
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
// CObjectDCLApp message handlers

void CObjectDCLApp::OnFileNew() 
{
	CloseAllDocuments(FALSE);
	CWinApp::OnFileNew();
}

void CObjectDCLApp::OnFileOpen() 
{
	CloseAllDocuments(FALSE);
	CWinApp::OnFileOpen();
}

BOOL CObjectDCLApp::OnOpenRecentFile(UINT nID)
{
	CloseAllDocuments(FALSE);
	return CWinApp::OnOpenRecentFile(nID);
}

CMDIChildWnd* CObjectDCLApp::CreateOrActivateFrame(CDocument* pDoc, CSize ViewSize, bool bResizable)
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

CObjectDCLView* CObjectDCLApp::OpenExistingForm(CDclFormObject *pDclForm)
{
	CRect rcNewDialog;
	CMDIChildWnd* pNewFrame;
	
	// get the arx object that holds the dialog's properties
	CDclControlObject *pDclProperties = pDclForm->GetControlProperties();
	CSize StartupSize(pDclProperties->GetLngProperty(nWidth), pDclProperties->GetLngProperty(nHeight));
	
	// create the new view
	bool bResizable = true;
	if (pDclForm->GetType() == VdclConfigTab || pDclForm->GetType() == VdclTabForm)
		bResizable = false;

	if (pDclForm->GetType() == VdclTabForm)
	{
		CDclFormObject *pParent = pDclForm->GetParentForm();
		if (pParent != NULL)
		{
			CDclControlObject* pTabCtrl = pParent->FindFirstControlOfType(CtlTabStrip);
			assert( pTabCtrl != NULL );
			if( pTabCtrl )
			{
				StartupSize.cx = pTabCtrl->GetLngProperty(nWidth);			
				StartupSize.cy = pTabCtrl->m_ClientHeight + GetSystemMetrics(SM_CYCAPTION);
			}
		}
		else 
			return NULL;
	}
	
	pNewFrame = CreateOrActivateFrame(theWorkspace.GetActiveDocument()/*m_pMainFrame->GetActiveDocument()*/, StartupSize, bResizable);
	CObjectDCLView* pNewView = (CObjectDCLView*)pNewFrame->GetActiveView();

	// set the dcl pointers
	pNewView->m_pThisDclForm = pDclForm;
	pDclForm->m_pChildWnd = pNewView;
	pDclForm->m_pMdiChildWnd = pNewFrame;

	ASSERT(pNewView != NULL);
	ASSERT(pNewView->IsKindOf(RUNTIME_CLASS(CObjectDCLView)));

	// get the position of the new dialog box
	pNewFrame->GetWindowRect(rcNewDialog);

	if (pDclForm->UsesClientRect())
	{
		CRect rcView;
		pNewView->GetClientRect(&rcView);
		rcNewDialog.right += rcNewDialog.Width() - rcView.Width();
		rcNewDialog.bottom += rcNewDialog.Height() - rcView.Height();
		pNewFrame->GetParent()->ScreenToClient(&rcNewDialog);
		pNewFrame->MoveWindow( &rcNewDialog, TRUE );
	}

	if (pDclForm != NULL)
	{
		if (pDclForm->GetType() == VdclFileDialog)
			((CChildFrame*)pNewFrame)->SetTitleBarIcon(-1);
		else
			((CChildFrame*)pNewFrame)->SetTitleBarIcon(pDclForm->GetDclFormTitleBarIcon());
		if (pDclForm->GetType() != VdclTabForm)
			((CChildFrame*)pNewFrame)->m_Title = pDclForm->GetDclFormTitle();
		else
			((CChildFrame*)pNewFrame)->m_Title = FindTabCaption(pDclForm);
		pNewFrame->SetWindowText(((CChildFrame*)pNewFrame)->m_Title);
	}
    
	// call the method to display the controls to the user
	pNewView->DisplayControls(pDclForm);
	if( !pNewView->m_SelectedControl.m_pArxObject )
		pNewView->m_SelectedControl.m_pArxObject = pDclProperties;
	theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties( pDclProperties, pNewView );
	return pNewView;
}

void CObjectDCLApp::OnFileCreatedistfile() 
{
	CString sFilter;
	sFilter = theWorkspace.LoadResourceString(IDS_ODCFILTER);

	// create the open dialog box
	CFileDialog BrowseWnd(
		TRUE, 
		NULL,
		NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		sFilter,
		CWnd::GetActiveWindow());

	// proceed to setup the file buffer size
	BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
	TCHAR* pc = new TCHAR[MAX_PATH];
	BrowseWnd.m_ofn.lpstrFile = pc;
    BrowseWnd.m_ofn.lpstrFile[0] = NULL;

	CString sText;	
	sText = theWorkspace.LoadResourceString(IDS_SELECTTOIMPORT);

	BrowseWnd.m_ofn.lpstrTitle = sText;

	// call method to invoke the file dialog box	
	int iReturn = BrowseWnd.DoModal();
	if(iReturn == IDOK)   
	{
		CProjectCollection phProjects;

		CString sPathName;
		POSITION pos;

		// do loop to get all selected files
		for (pos = BrowseWnd.GetStartPosition(); pos != NULL; )
		{
			// get the file name 
			sPathName = BrowseWnd.GetNextPathName(pos);
			CProject *pProject = new CProject;
			if( pProject->ReadFromFile(sPathName) == statOK )
				phProjects.AddProject(pProject);
			else
				delete pProject;
		}

		SaveDistributionFile(&phProjects);
		phProjects.ClearProjects();
	}

	delete [] pc; 
}

BOOL CObjectDCLApp::SaveDistributionFile(CProjectCollection *pProjectHolder) 
{
	CString sFilter; 
	sFilter = theWorkspace.LoadResourceString(IDS_ODSFILTER);

	CString sText;	
	sText = theWorkspace.LoadResourceString(IDS_STARODS);

	// create the open dialog box
	CFileDialog BrowseWnd(
		FALSE, 
		sText,
		NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_PATHMUSTEXIST,
		sFilter,
		CWnd::GetActiveWindow());

	sText = theWorkspace.LoadResourceString(IDS_SAVEDISTAS);

	BrowseWnd.m_ofn.lpstrTitle = sText;
	// proceed to setup the file buffer size
	BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
	TCHAR* pc = new TCHAR[MAX_PATH];
	BrowseWnd.m_ofn.lpstrFile = pc;
    BrowseWnd.m_ofn.lpstrFile[0] = NULL;

	// call method to invoke the file dialog box
	int iReturn = BrowseWnd.DoModal();

	if(iReturn == IDOK)   
	{
		CString sFileName = BrowseWnd.m_ofn.lpstrFile;
	
		// declare file variables
		CFileException Exception;
		CFile ThisFile;
		CString sTitle;
		sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
		
		// open then file
		if ( !ThisFile.Open(sFileName, CFile::modeCreate | CFile::shareExclusive | CFile::modeWrite, &Exception) )
		{
			CString sTheFile;	
			sTheFile = theWorkspace.LoadResourceString(IDS_THEFILE);
			CString sText;	
			sText = theWorkspace.LoadResourceString(IDS_COULDNOTOPEN);

			MessageBox (::GetActiveWindow(), sTheFile + sFileName + sText, sTitle, NULL);
			return FALSE;
		}
		
		CArchiveEx arExt(&ThisFile, CArchive::store | CArchive::bNoFlushOnDelete, NULL, _T("ObjectDCL"), TRUE);
		// Serialize the control
		pProjectHolder->Serialize(arExt);
		arExt.Close();
		ThisFile.Close();
	}
	delete [] pc; 
	return TRUE;
}

CObjectDCLView* CObjectDCLApp::AddDclFormAndView(DclFormType nType) 
{
	CRect rcNewDialog;
	CMDIChildWnd* pNewFrame;

	CDclFormObject* pNewDcl = AddNewDclForm(nType);
	CDclControlObject *pDclProperties = pNewDcl->GetControlProperties();
	assert(pDclProperties != NULL);

	// get the arx object that holds the dialog's properties
	CSize StartupSize(pDclProperties->GetLngProperty(nWidth), pDclProperties->GetLngProperty(nHeight));
	
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
			((CChildFrame*)pNewFrame)->SetTitleBarIcon(nNotSet); // set the title bar icon.
	}

	CObjectDCLView* pNewView = (CObjectDCLView*)pNewFrame->GetActiveView();

	// set the dcl pointers
	pNewDcl->m_pChildWnd = pNewView;
	pNewDcl->m_pMdiChildWnd = pNewFrame;

	ASSERT(pNewView != NULL);
	ASSERT(pNewView->IsKindOf(RUNTIME_CLASS(CObjectDCLView)));

	// get the position of the new dialog box
	pNewFrame->GetWindowRect(rcNewDialog);
	pNewFrame->GetParent()->ScreenToClient(rcNewDialog);
	
	// reset the size of the new dialog box
	rcNewDialog.right = rcNewDialog.left + pDclProperties->GetLngProperty(nWidth);
	rcNewDialog.bottom = rcNewDialog.top + pDclProperties->GetLngProperty(nHeight);

	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
	pZOrderList->ClearSelection();

	if (pNewDcl != NULL)
		// set the title bar text
		pNewFrame->SetWindowText(pNewDcl->GetDclFormTitle());
	// set the dcl pointer in the view
	pNewView->m_pThisDclForm = pNewDcl;
	if (pNewDcl->GetType() == VdclFileDialog && pNewDcl->GetControlCount() == 1)
	{
		CRect rc(0,0,416,258);
		pNewView->InsertControl(rc, CtlFileDlgCtrl);
	}
	else
	{
		// call the method to display the controls to the user
		pNewView->DisplayControls(pNewDcl);
	}
	if( !pNewView->m_SelectedControl.m_pArxObject )
		pNewView->m_SelectedControl.m_pArxObject = pDclProperties;
	theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties( pDclProperties, pNewView );

	if (pNewDcl->m_htiTreeItem != NULL)
	{
		CProjectTreeCtrl *pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
		pProjTree->SelectItem(pNewDcl->m_htiTreeItem);
	}
	return pNewView;
}

CDclFormObject* CObjectDCLApp::AddNewDclForm(DclFormType nType) 
{
	// create a pointer to pass to the list to insert
	CDclFormObject* pNewDclForm = activeProject->AddForm( nType );
	if( pNewDclForm )
	{	
		AddDefaultProperties(pNewDclForm->GetControlProperties(), -1, -1); //add properties to the new dcl form object
		CProjectTreeCtrl *pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
		if( pProjTree )
			pProjTree->AddFormToTree(pNewDclForm, true); //add the new dcl form to the project tree
	}
	return pNewDclForm;
}

void CObjectDCLApp::OnToolsDefaultfont() 
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

int CObjectDCLApp::ExitInstance() 
{
	if (m_pCtrlHelp != NULL)
		delete m_pCtrlHelp;
	return CWinApp::ExitInstance();
}

void CObjectDCLApp::OnToolsGridspacing() 
{
	CGridSpacingDlg Dlg;
	Dlg.DoModal();
}

void CObjectDCLApp::OnUpdateToolsEventscopytoclipboard(CCmdUI* pCmdUI) 
{
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);

	CString sText;
	sText = theWorkspace.LoadResourceString(IDS_EventsCopyToClipboard);

    BOOL bUsesOn = pApp->GetProfileInt(sProfileName, sText, TRUE);
    
	pCmdUI->SetCheck(bUsesOn);	
}

void CObjectDCLApp::OnUpdateToolsEventswritetolispfile(CCmdUI* pCmdUI) 
{
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);

    CString sText;
	sText = theWorkspace.LoadResourceString(IDS_EventsWriteToLispFile);

	BOOL bUsesOn = pApp->GetProfileInt(sProfileName, sText, FALSE);
    
	pCmdUI->SetCheck(bUsesOn);	
}

void CObjectDCLApp::OnToolsEventscopytoclipboard() 
{
	CString sSection = theWorkspace.LoadResourceString(IDR_MAINFRAME);
	CString sName = theWorkspace.LoadResourceString(IDS_EventsCopyToClipboard);
	BOOL bCurrentVal = AfxGetApp()->GetProfileInt(sSection, sName, TRUE);
	AfxGetApp()->WriteProfileInt(sSection, sName, !bCurrentVal);

	CString sText = theWorkspace.LoadResourceString(IDS_COPYTOCLIPBOARD);
	theEditorWorkspace.GetPropertyTabs()->m_EventsTabPane.m_AddToLisp.SetWindowText(sText);
}

void CObjectDCLApp::OnToolsEventswritetolispfile() 
{
	CString sSection = theWorkspace.LoadResourceString(IDR_MAINFRAME);
	CString sName = theWorkspace.LoadResourceString(IDS_EventsWriteToLispFile);
	BOOL bCurrentVal = AfxGetApp()->GetProfileInt(sSection, sName, TRUE);
	AfxGetApp()->WriteProfileInt(sSection, sName, !bCurrentVal);
    
	CString sText = theWorkspace.LoadResourceString(IDS_ADDTOSTRING);
	theEditorWorkspace.GetPropertyTabs()->m_EventsTabPane.m_AddToLisp.SetWindowText(sText);	
}


void CObjectDCLApp::OnToolsExportlanguageexcelspreadsheet() 
{
}

void CObjectDCLApp::OnHelp() 
{
	HtmlHelp(0, HH_DISPLAY_TOPIC);
}

void CObjectDCLApp::OnHelpFinder() 
{
	//CHelp1 Dlg;
	//Dlg.DoModal();

	//if (Dlg.m_nType == 1)
	//{
	//	if (m_pCtrlHelp == NULL)
	//	{
	//		m_pCtrlHelp = new CHelp2();
	//		m_pCtrlHelp->Create(MAKEINTRESOURCE(IDD_HELPCTRLS), m_pMainWnd);
	//	}
	//	m_pCtrlHelp->ShowWindow(TRUE);
	//}
	//if (Dlg.m_nType == 2)
	//{

	//	// look in the application directory for the method file
	//	TCHAR drive[_MAX_DRIVE];
	//	TCHAR dir[_MAX_DIR];
	//	TCHAR fname[_MAX_FNAME];
	//	TCHAR ext[_MAX_EXT];
	//	LPCTSTR path =
	//#ifdef _UNICODE
	//		_wpgmptr ;
	//#else
	//		_pgmptr ;
	//#endif
	//	_tsplitpath(path, drive, dir, fname, ext );

	//	::ShellExecute(
	//		NULL, 
	//		_T("open"), 
	//		CString(drive) + dir + _T("ObjectDcl.hlp"),
	//		NULL, 
	//		NULL,
	//		SW_SHOWNORMAL);
	//}
	//if (Dlg.m_nType == 3)
	//{

	//	// look in the application directory for the method file
	//	TCHAR drive[_MAX_DRIVE];
	//	TCHAR dir[_MAX_DIR];
	//	TCHAR fname[_MAX_FNAME];
	//	TCHAR ext[_MAX_EXT];
	//	LPCTSTR path =
	//#ifdef _UNICODE
	//		_wpgmptr ;
	//#else
	//		_pgmptr ;
	//#endif
	//	_tsplitpath(path, drive, dir, fname, ext );

	//	MessageBox(m_pMainWnd->m_hWnd, _T("This may take a minute to open."), _T("Help"), NULL);

	//	::ShellExecute(
	//		NULL, 
	//		_T("open"), 
	//		CString(drive) + dir + _T("AU.doc"),
	//		NULL, 
	//		NULL,
	//		SW_SHOWNORMAL);

	//	
	//}

	//if (Dlg.m_nType == 4)
	//{
	//	CDclFormObject DclForm(NULL, (DclFormType)-2); // -2 to indicate bonus functions
	//	RefCountedPtr< COleControlObject > pOleControl = new COleControlObject( (ControlType)-2 ); // -2 to indicate bonus functions
	//	RefCountedPtr< CPropertyObject > pProp = new CPropertyObject(PropInvalid, 0, nName);
	//	pOleControl->GetPropertyList().AddTail(pProp);
	//	
	//	CObjectBrowser Dlg(m_pMainFrame);

	//	Dlg.m_pControl = pOleControl;
	//	Dlg.m_pDclForm = &DclForm;
	//	Dlg.m_sDclFormName.Empty();

	//	Dlg.DoModal();
	//}
}
