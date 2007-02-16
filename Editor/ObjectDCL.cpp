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
#include "SharedRes.h"


static CString LTOA(int nVal)
{
  CString sLong;
	sLong.Format(_T("%d"), nVal);
  return sLong;
}


//static CDclControlObject* GetArxTabControlObject(CDclFormObject *pDclForm)
//{
//	CDclControlObject *pRetObject;
//
//	// create a position variable to hold the converted ArxControlIndex
//	POSITION ControlPos;
//	
//	// set the position variable to be equal the index to passing to the GetAt method
//	ControlPos = pDclForm->m_ArxControlList.GetHeadPosition();	
//	while (ControlPos != NULL)
//	{
//		// set the pass pointer to point at the object in the list
//		pRetObject = pDclForm->m_ArxControlList.GetNext(ControlPos);
//		if (pRetObject != NULL && pRetObject->GetType() == CtlTabStrip)
//			return pRetObject;
//	}
//
//	return NULL;
//		
//}


static CString FindTabCaption2(CDclFormObject *pDclTab, int nTabIndex)
{
	CDclControlObject* pControl = pDclTab->FindFirstControlOfType(CtlTabStrip);
	if (!pControl)
		return CString();
	return pControl->GetPropertyListItem(nTabsCaption, nTabIndex);
}


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
					return FindTabCaption2(pDcl, pDclTab->GetTabIndex());
				}
			}			
		}
	}
	return CString();
}


static RefCountedPtr< CPropertyObject > AddControlStdProperty(CDclControlObject *pDclControl, PropertyId nID, LPCTSTR pszValue, PropertyType type, bool bHidden = false) 
{
	RefCountedPtr< CPropertyObject > pProp = pDclControl->AddStringProperty( nID, type, pszValue );
	if( bHidden )
		pProp->SetHidden();
	return pProp;
}


static RefCountedPtr< CPropertyObject > AddControlHiddenProperty(CDclControlObject *pDclControl, PropertyId nID, LPCTSTR pszValue, PropertyType type) 
{
	return AddControlStdProperty(pDclControl, nID, pszValue, type, true);
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
	//ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
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

	m_pCtrlHelp = NULL;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
   
	CString s3rdDay;	
	s3rdDay = theWorkspace.LoadResourceString(IDS_3RDDAY);

	// Change the registry key under which our settings are stored.
	SetRegistryKey(s3rdDay);

	LoadStdProfileSettings(nNumberOfPrevFiles);  // Load standard INI file options (including MRU)

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

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CObjectDCLApp message handlers

/*
CDocument* CObjectDCLApp::OpenDocumentFile(LPCTSTR pszPathName)
{
  CDocument* pDoc = NULL;
  POSITION pos = GetFirstDocTemplatePosition();
  ASSERT(pos != NULL);
  // the first doctemplate is the main view of the document
  CDocTemplate* pTemplate = GetNextDocTemplate(pos);
  if (CloseDocument())
  {
    pDoc = pTemplate->OpenDocumentFile(pszPathName);
		if (pDoc != NULL)
		{
			if( pszPathName )
				pDoc->SetTitle(pszPathName);
			else
			{
				CString sTitle;
				sTitle = theWorkspace.LoadResourceString(IDS_UNTITILED);
				pDoc->SetTitle(sTitle);
			}
			CProject *pProject = activeProject;
			if (pProject != NULL)
			{
				pProject->GetKeyName() = pszPathName;
				pProject->ClearProject();	
				theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
			}
			POSITION pos = pDoc->GetFirstViewPosition();
			if( pos )
				m_pMainFrame->SetActiveView(pDoc->GetNextView(pos), FALSE);
		}
  }

	return pDoc;
}
*/

void CObjectDCLApp::OnFileNew() 
{
	CWinApp::OnFileNew();
	m_pMainFrame->m_wndToolBar.GetToolBarCtrl().EnableButton(0, TRUE);
}

void CObjectDCLApp::OnFileOpen() 
{
	CloseDocument();
	CWinApp::OnFileOpen();
/*
	//CString newName;
	//if (!DoPromptFileName(newName, IDS_OPENPROJECTFILE,
	//	OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ENABLESIZING, TRUE, NULL)) return;
	//
	//if (m_pDoc != NULL)
	//{
	//	if (!m_pDoc->CheckForSaveModified())
	//		return;
	//}

	//CloseDocument();
	//
	//	
	//CDocument *pDoc = CObjectDCLApp::OpenDocumentFile(newName);
	//m_pMainFrame->GetActiveFrame()->ShowWindow(FALSE);		
	//
	//if (m_pMainFrame != NULL)
	//	m_pMainFrame->m_ProjectTree.SetupProjectTree(pProject);
	//
	//CString sBakFileName = newName;
	//sBakFileName.Replace(_T(".odc"), _T("._od"));
	//::CopyFile(newName, sBakFileName, FALSE);
*/
	//CProject *pProject = activeProject;
	//if (pProject != NULL)
	//{
	//	theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
	//	pProject->ClearProject();	
	//}
	GetMainFrame()->m_wndToolBar.GetToolBarCtrl().EnableButton(0, TRUE);
}


BOOL CObjectDCLApp::OnOpenRecentFile(UINT nID)
{
	BOOL bSuccess = CWinApp::OnOpenRecentFile(nID);
	if( bSuccess )
	  m_pMainFrame->m_wndToolBar.GetToolBarCtrl().EnableButton(0, TRUE);
	return bSuccess;

/*
	// lets ensure the file is actually on the disk
	int nIndex = nID - ID_FILE_MRU_FILE1;	
	CFileFind find;
	if (!find.FindFile((*m_pRecentFileList)[nIndex]))
		return FALSE;

	// lets prompt the user is he wants to save
	if (m_pDoc != NULL)
	{
		if (!m_pDoc->CheckForSaveModified())
			return FALSE;
	}

	// start opening the file
  if (CloseDocument())
  {
	CProject *pProjectList = activeProject;
	if (pProjectList != NULL)
	{
		theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
		pProjectList->ClearProject();	
	}
		
    ASSERT_VALID(this);
	ASSERT(m_pRecentFileList != NULL);

	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_pRecentFileList->GetSize());
	nIndex = nID - ID_FILE_MRU_FILE1;
	ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);

	CFileFind find;
	if (!find.FindFile((*m_pRecentFileList)[nIndex]))
		return FALSE;

	CDocument *pDoc = OpenDocumentFile((*m_pRecentFileList)[nIndex]);
	if (pDoc == NULL)
		m_pRecentFileList->Remove(nIndex);

  }
  m_pMainFrame->m_wndToolBar.GetToolBarCtrl().EnableButton(0, TRUE);
  return TRUE;
*/
}

BOOL CObjectDCLApp::CloseDocument()
{
	// Find first doc template
    POSITION pos = GetFirstDocTemplatePosition();
    ASSERT(pos != NULL);
    CDocTemplate* pTemplate = GetNextDocTemplate(pos);

    // get the document
    pos = pTemplate->GetFirstDocPosition();
    if (pos != NULL)
    {
			//m_pMainFrame->SetActiveView(NULL, FALSE);
      CDocument* pDoc = pTemplate->GetNextDoc(pos);
      // remove Document (only 1 possible)
      if (pDoc != NULL) pDoc->OnCloseDocument();
    }
   
	if (m_pMainFrame != NULL)
	{
		theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
		CProject *pProjectList = activeProject;
		if (pProjectList != NULL)
		{
			theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
			pProjectList->ClearProject();	
		}
		m_pMainFrame->m_wndToolBar.GetToolBarCtrl().EnableButton(0, FALSE);
	}
    return TRUE;
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
		pNewFrame->ModifyStyle(WS_THICKFRAME, DS_MODALFRAME, NULL);
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

	CRect rcClient;
	// get the position of the new dialog box
	pNewFrame->GetWindowRect(rcNewDialog);
	pNewFrame->GetClientRect(&rcClient);

	if (pDclForm->m_bUsesClientRect == TRUE)
	{
		StartupSize.cx += rcNewDialog.Width() - rcClient.Width();
		StartupSize.cy += rcNewDialog.Height() - rcClient.Height();
	}
	pDclForm->m_bUsesClientRect = TRUE;
	
	pNewFrame->GetParent()->ScreenToClient(rcNewDialog);

	if (pDclForm != NULL)
	{
		if (pDclForm->GetType() == VdclFileDialog)
			// set the title bar icon.
			((CChildFrame*)pNewFrame)->SetTitleBarIcon(-1);
		else
			// set the title bar icon.
			((CChildFrame*)pNewFrame)->SetTitleBarIcon(pDclForm->GetDclFormTitleBarIcon());
	}
	
	if (pDclForm->GetType() == VdclTabForm)
	{	
		// reset the size of the new dialog box
		rcNewDialog.right = rcNewDialog.left + StartupSize.cx + 1;
		rcNewDialog.bottom = rcNewDialog.top + StartupSize.cy + 1;		
	}
	else
	{
		// reset the size of the new dialog box
		rcNewDialog.right = rcNewDialog.left + StartupSize.cx;
		rcNewDialog.bottom = rcNewDialog.top + StartupSize.cy;
	}

	// update the new dialog box's size
	pNewFrame->MoveWindow(rcNewDialog, TRUE);

	if (pDclForm != NULL)
	{
		if (pDclForm->GetType() != VdclTabForm)
		{
			// set the title bar text
			((CChildFrame*)pNewFrame)->m_Title = pDclForm->GetDclFormTitle();
			pNewFrame->SetWindowText(((CChildFrame*)pNewFrame)->m_Title);
		}
		else
		{
			// set the title bar text
			((CChildFrame*)pNewFrame)->m_Title = FindTabCaption(pDclForm);
			pNewFrame->SetWindowText(((CChildFrame*)pNewFrame)->m_Title);
		}		
	}

	// add additional properties that may not be there
	if (pDclForm->GetType() != VdclTabForm)
	{	
		CString sUnderscore;
		sUnderscore = theWorkspace.LoadResourceString(IDS_UNDERSCORE);

		// add the nObjectBrowser property
		AddControlStdProperty(pDclForm->GetControlProperties(), nObjectBrowser, CString(), PropActiveXMethods);

		// add the GlobalVarName property
		CString sVarName = theEditorWorkspace.GetActiveProjectName() + sUnderscore + pDclForm->GetKeyName();
		AddControlStdProperty(pDclForm->GetControlProperties(), nGlobalVarName, sVarName, PropString);
	}

	// add additional properties that may not be there
	if (pDclForm->GetType() == VdclDockable)
	{		
		// add TitleBarText property
		AddControlStdProperty(pDclForm->GetControlProperties(), nTitleBarText, pDclForm->GetKeyName(), PropString);
		
		if (nCurrentPurchaseMode != nPurchasedR14Pro && 
			pDclForm->GetControlProperties()->GetPropertyObject(nResizable) == NULL)
		{
			CString sTrue;		
			sTrue = theWorkspace.LoadResourceString(IDS_TRUE);
	
			// add the allow user to resize property
			AddControlStdProperty(pDclForm->GetControlProperties(), nResizable, sTrue, PropBool);
		}		
	}
    
	// set the dcl pointer in the view
	pNewView->m_pThisDclForm = pDclForm;
	// call the method to display the controls to the user
	pNewView->DisplayControls(pDclForm);
	
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


/*
BOOL CObjectDCLApp::OpenProject(LPCTSTR FileName, CProject *pProject) 
{
	// declare file variables
	CFileException Exception;
	CFile ThisFile;
	
	
	// open then file
	if (!ThisFile.Open(FileName, CFile::modeRead | CFile::shareDenyWrite, &Exception))
	{
		return FALSE;
	}
	
	CString sTitle;
	sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);

		

	CArchiveEx ar(&ThisFile, CArchive::load | CArchive::bNoFlushOnDelete, NULL, sTitle, TRUE);
		
	try
	{
		// Serialize the control
		pProject->Serialize(ar);
		ar.Close();
	}
	catch(...)
	{
		ar.Close();
		return FALSE;
	}

	ThisFile.Close();

	CString sShortName = FileName;
	sShortName.MakeReverse;
	sShortName = sShortName.SpanExcluding(_T("\\/:"));
	sShortName.MakeReverse();
	sShortName = sShortName.SpanExcluding(_T("."));
	pProject->GetKeyName() = sShortName;
	
	if (nCurrentPurchaseMode != nPurchasedR14Pro)
		pProject->ClearR14Events();
	
	return TRUE;
}
*/


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
	if (nType == VdclConfigTab || nType == VdclTabForm)
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
		
	// update the new dialog box's size
//	pNewFrame->MoveWindow(rcNewDialog, TRUE);

	if (pNewDcl != NULL)
		// set the title bar text
		pNewFrame->SetWindowText(pNewDcl->GetDclFormTitle());
	// set the dcl pointer in the view
	pNewView->m_pThisDclForm = pNewDcl;
	if (pNewDcl->GetType() == VdclFileDialog && pNewDcl->GetControlCount() == 1)
	{
		CRect rc(0,0,nCtlFileWidth,nCtlFileHeight);
		pNewView->InsertControl(rc, CtlFileDlgCtrl);
		;
		//pNewView->MoveThisInPosition();		
	}
	else
	{
		// call the method to display the controls to the user
		pNewView->DisplayControls(pNewDcl);
	}
	
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
		AddDclFormProperties(pNewDclForm, nType); //add properties to the new dcl form object
		CProjectTreeCtrl *pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
		if( pProjTree )
			pProjTree->AddFormToTree(pNewDclForm, true); //add the new dcl form to the project tree
	}
	return pNewDclForm;
}


void CObjectDCLApp::AddDclFormProperties(CDclFormObject *pNewDclForm, DclFormType nType) 
{
	CString sText;
	CString sTrue;
	CString sFalse;
	sTrue = theWorkspace.LoadResourceString(IDS_TRUE);
	sFalse = theWorkspace.LoadResourceString(IDS_FALSE);
	CProject *pProject = activeProject;

	// create a new arx object to hold the new dcl form's properties
	CDclControlObject* pArxPropertyObject = pNewDclForm->GetControlProperties();

	// add standard properties

	// lets create a name for the new dcl form
	CString sFormName;
	sFormName.Format( _T("%s%d"), theWorkspace.LoadResourceString(IDS_DCLFORM), pProject->GetDclFormList().GetCount() );
	AddControlStdProperty(pArxPropertyObject, nName, sFormName, PropString); // add the name property

	//original ODCL 3 code created a default global lisp symbol name; this has been removed (commented below) 
	//in favor of an empty value in order to use the default name calculated at runtime   2007-02-15 [ORW]
	CString sGlobalVarName/* = CString( pProject->GetKeyName() ) + _T('_') + sFormName*/;
	AddControlStdProperty(pArxPropertyObject, nGlobalVarName, sGlobalVarName, PropString);

	// add the nObjectBrowser property
	AddControlStdProperty(pArxPropertyObject, nObjectBrowser, CString(), PropActiveXMethods);
	
	// add properties per their dcl form type
	switch (nType)
	{
	case VdclFileDialog:
		// add the allow user to resize property
		AddControlStdProperty(pArxPropertyObject, nResizable, sTrue, PropBool);
		// add the height property with a default width
		AddControlStdProperty(pArxPropertyObject, nWidth, LTOA(nDeCtlFileWidth), PropLong);
		// add the height property with a default height
		AddControlStdProperty(pArxPropertyObject, nHeight, LTOA(nDeCtlFileHeight+6), PropLong);
		    
		sText = theWorkspace.LoadResourceString(IDS_OPEN);
		// add TitleBarText property
		AddControlStdProperty(pArxPropertyObject, nTitleBarText, sText /*"Open"*/, PropString);
                 
		// add the events to the property list
		AddControlHiddenProperty(pArxPropertyObject, nFormEventInitialize, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nFormEventClose, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nFormEventSize, CString(), PropEvent);
		break;
	case VdclModal:
		// add the allow user to resize property
		AddControlStdProperty(pArxPropertyObject, nResizable, sFalse, PropBool);
		// add the height property with a default width of 350
		AddControlStdProperty(pArxPropertyObject, nWidth, LTOA(nDeModalWidth), PropLong);
		// add the height property with a default height of 250
		AddControlStdProperty(pArxPropertyObject, nHeight, LTOA(nDeModalHeight), PropLong);
		
		// add the min and max properties
		AddControlStdProperty(pArxPropertyObject, nMinDialogWidth, LTOA(nDeMinSize), PropLong);
		AddControlStdProperty(pArxPropertyObject, nMinDialogHeight, LTOA(nDeMinSize), PropLong);
		AddControlStdProperty(pArxPropertyObject, nMaxDialogWidth, LTOA(nDeMaxSize), PropLong);
		AddControlStdProperty(pArxPropertyObject, nMaxDialogHeight, LTOA(nDeMaxSize), PropLong);
            
		sText = theWorkspace.LoadResourceString(IDS_NONE2);
		// add Icon property
		AddControlStdProperty(pArxPropertyObject, nIcon, sText /*"None" */, PropPicture);
            
		// add TitleBarText property
		AddControlStdProperty(pArxPropertyObject, nTitleBarText, sFormName, PropString);
                 
		// add the events to the property list
		AddControlHiddenProperty(pArxPropertyObject, nFormEventInitialize, CString(), PropEvent);
    AddControlHiddenProperty(pArxPropertyObject, nFormEventOnOk, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nFormEventClose, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nFormEventSize, CString(), PropEvent);
		if (nCurrentPurchaseMode != nPurchasedR14Pro)
			AddControlHiddenProperty(pArxPropertyObject, nFormEventCancelClose, CString(), PropEvent);
		break;
	case VdclModeless:
		// add the allow user to resize property
		AddControlStdProperty(pArxPropertyObject, nResizable, sTrue, PropBool);
		// add the height property with a default width of 250
		AddControlStdProperty(pArxPropertyObject, nWidth, LTOA(nDeModelessWidth), PropLong);
		// add the height property with a default height of 150
		AddControlStdProperty(pArxPropertyObject, nHeight, LTOA(nDeModelessHeight), PropLong);

		// add the min and max properties
		AddControlStdProperty(pArxPropertyObject, nMinDialogWidth, LTOA(nDeMinSize), PropLong);
		AddControlStdProperty(pArxPropertyObject, nMinDialogHeight, LTOA(nDeMinSize), PropLong);
		AddControlStdProperty(pArxPropertyObject, nMaxDialogWidth, LTOA(nDeMaxSize), PropLong);
		AddControlStdProperty(pArxPropertyObject, nMaxDialogHeight, LTOA(nDeMaxSize), PropLong);
            
		sText = theWorkspace.LoadResourceString(IDS_NONE2);
		// add Icon property
		AddControlStdProperty(pArxPropertyObject, nIcon, sText /*"None" */, PropPicture);
            
		// add TitleBarText property
		AddControlStdProperty(pArxPropertyObject, nTitleBarText, sFormName, PropString);
        
		// add the events to the property list
		AddControlHiddenProperty(pArxPropertyObject, nFormEventClose, CString(), PropEvent);
    AddControlHiddenProperty(pArxPropertyObject, nFormEventOnOk, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nFormEventInitialize, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nFormEventSize, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nDocEventActivated, CString(), PropEvent);
		if (nCurrentPurchaseMode != nPurchasedR14Pro)
			AddControlHiddenProperty(pArxPropertyObject, nFormEventCancelClose, CString(), PropEvent);		
            
		break;
	case VdclDockable:
		if (nCurrentPurchaseMode != nPurchasedR14Pro)
		{
			// add the allow user to resize property
			AddControlStdProperty(pArxPropertyObject, nResizable, sTrue, PropBool);
		}
		else
		{
			// add the allow user to min dialog width and height properties
			AddControlStdProperty(pArxPropertyObject, nMinDialogWidth, LTOA(nDeMinDialogWidth), PropLong);
			AddControlStdProperty(pArxPropertyObject, nMinDialogHeight, LTOA(nDeMinDialogHeight), PropLong);
		}
		
		// add TitleBarText property
		AddControlStdProperty(pArxPropertyObject, nTitleBarText, sFormName, PropString);
        
		// add the dockable sides property
		AddControlStdProperty(pArxPropertyObject, nDockableSides, LTOA(0), PropEnum);
		// add the height property with a default height of 150
		AddControlStdProperty(pArxPropertyObject, nHeight, LTOA(nDeDockableHeight), PropLong);
		// add the height property with a default width of 250
		AddControlStdProperty(pArxPropertyObject, nWidth, LTOA(nDeDockableWidth), PropLong);
		
		// add the events to the property list
		AddControlHiddenProperty(pArxPropertyObject, nFormEventClose, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nFormEventInitialize, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nFormEventSize, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nDocEventActivated, CString(), PropEvent);
		                
		break;
	case VdclConfigTab:
		// add the caption properties
		AddControlStdProperty(pArxPropertyObject, nCfgTabCaption, sFormName, PropString);
		// add the height property with a default height of 380
		AddControlStdProperty(pArxPropertyObject, nHeight, LTOA(nDeConfigTabHeight), PropLong);
		// add the height property with a default width of 600
		AddControlStdProperty(pArxPropertyObject, nWidth, LTOA(nDeConfigTabWidth), PropLong);
		
		// add the events to the property list
		AddControlHiddenProperty(pArxPropertyObject, nFormEventInitialize, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nFormEventShow, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nCfgEventCancel, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nCfgEventHelp, CString(), PropEvent);
		AddControlHiddenProperty(pArxPropertyObject, nCfgEventOK, CString(), PropEvent);
		
		break;
	case VdclTabForm:
		// add the height property with a default height of 150
		AddControlStdProperty(pArxPropertyObject, nHeight, LTOA(nDeModelessHeight), PropLong);
		// add the height property with a default width of 250
		AddControlStdProperty(pArxPropertyObject, nWidth, LTOA(nDeModelessWidth), PropLong);
		break;
	}	 
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

/*
void CObjectDCLApp::OnFileClose() 
{
	CloseDocument();	

	m_pMainFrame->m_wndDlgBar.m_FontNames.EnableWindow(FALSE);
	m_pMainFrame->m_wndDlgBar.m_FontSizes.EnableWindow(FALSE);
	m_pMainFrame->m_wndToolBar.EnableWindow(FALSE);
		
}

BOOL CObjectDCLApp::SaveAllModified() 
{
	// lets prompt the user is he wants to save
	if (m_pDoc != NULL)
	{
		if (!m_pDoc->CheckForSaveModified())
			return FALSE;
	}
	return TRUE;
}
*/

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
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);

	CString sText;
		
	sText = theWorkspace.LoadResourceString(IDS_EventPrefixUsesON);
    BOOL bUsesOn = pApp->GetProfileInt(sProfileName, sText, TRUE);
	sText = theWorkspace.LoadResourceString(IDS_EventsCopyToClipboard);
	pApp->WriteProfileInt(sProfileName, sText, !bUsesOn);
	sText = theWorkspace.LoadResourceString(IDS_EventsWriteToLispFile);
    pApp->WriteProfileInt(sProfileName, sText, bUsesOn);

	sText = theWorkspace.LoadResourceString(IDS_COPYTOCLIPBOARD);
	theEditorWorkspace.GetPropertyTabs()->m_EventsTabPane.m_AddToLisp.SetWindowText(sText);
}

void CObjectDCLApp::OnToolsEventswritetolispfile() 
{
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);

	CString sText;
		
	sText = theWorkspace.LoadResourceString(IDS_EventsWriteToLispFile);    
    BOOL bUsesOn = pApp->GetProfileInt(sProfileName, sText, TRUE);
	pApp->WriteProfileInt(sProfileName, sText, !bUsesOn);
	sText = theWorkspace.LoadResourceString(IDS_EventsCopyToClipboard);
	pApp->WriteProfileInt(sProfileName, sText, bUsesOn);
    
	sText = theWorkspace.LoadResourceString(IDS_ADDTOSTRING);
	theEditorWorkspace.GetPropertyTabs()->m_EventsTabPane.m_AddToLisp.SetWindowText(sText);	
}


void CObjectDCLApp::OnToolsExportlanguageexcelspreadsheet() 
{
	
	
}

void CObjectDCLApp::OnHelp() 
{
}

void CObjectDCLApp::OnHelpFinder() 
{
	CHelp1 Dlg;
	Dlg.DoModal();

	if (Dlg.m_nType == 1)
	{
		if (m_pCtrlHelp == NULL)
		{
			m_pCtrlHelp = new CHelp2();
			m_pCtrlHelp->Create(MAKEINTRESOURCE(IDD_HELPCTRLS), m_pMainWnd);
		}
		m_pCtrlHelp->ShowWindow(TRUE);
	}
	if (Dlg.m_nType == 2)
	{

		// look in the application directory for the method file
		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];
		LPCTSTR path =
	#ifdef _UNICODE
			_wpgmptr ;
	#else
			_pgmptr ;
	#endif
		_tsplitpath(path, drive, dir, fname, ext );

		::ShellExecute(
			NULL, 
			_T("open"), 
			CString(drive) + dir + _T("ObjectDcl.hlp"),
			NULL, 
			NULL,
			SW_SHOWNORMAL);
	}
	if (Dlg.m_nType == 3)
	{

		// look in the application directory for the method file
		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];
		LPCTSTR path =
	#ifdef _UNICODE
			_wpgmptr ;
	#else
			_pgmptr ;
	#endif
		_tsplitpath(path, drive, dir, fname, ext );

		MessageBox(m_pMainWnd->m_hWnd, _T("This may take a minute to open."), _T("Help"), NULL);

		::ShellExecute(
			NULL, 
			_T("open"), 
			CString(drive) + dir + _T("AU.doc"),
			NULL, 
			NULL,
			SW_SHOWNORMAL);

		
	}

	if (Dlg.m_nType == 4)
	{
		CDclFormObject DclForm(NULL, (DclFormType)-2); // -2 to indicate bonus functions
		RefCountedPtr< COleControlObject > pOleControl = new COleControlObject(NULL); // -2 to indicate bonus functions
		RefCountedPtr< CPropertyObject > pProp = new CPropertyObject(PropInvalid, 0, nName);
		pOleControl->GetPropertyList().AddTail(pProp);
		
		CObjectBrowser Dlg(m_pMainFrame);

		Dlg.m_pControl = pOleControl;
		Dlg.m_pDclForm = &DclForm;
		Dlg.m_sDclFormName = CString();

		Dlg.DoModal();
	}
}
