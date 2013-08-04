// OpenDCL.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OpenDCL.h"
#include "StudioFrame.h"
#include "OpenDCLDoc.h"
#include "StdioUnicodeFile.h"
#include "ProjectPane.h"
#include "StudioWorkspace.h"
#include "FontSettings.h"
#include "FontPropPage.h"
#include "GridSpacingDlg.h"
#include "HtmlBrowser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Constant strings
#define S_DefaultFontName _T("DefaultFontName")
#define S_DefaultFontSize _T("DefaultFontSize")
#define S_DefaultFontItalic _T("DefaultFontItalic")
#define S_DefaultFontUnderLine _T("DefaultFontUnderLine")
#define S_DefaultFontBold _T("DefaultFontBold")
#define S_DefaultFontSizeStyle _T("DefaultFontSizeStyle")


// COpenDCLApp

BEGIN_MESSAGE_MAP(COpenDCLApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &COpenDCLApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_CLOSE, &COpenDCLApp::OnFileClose)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
	ON_COMMAND(ID_TOOLS_DEFAULTFONT, OnToolsDefaultfont)
	ON_COMMAND(ID_TOOLS_GRIDSPACING, OnToolsGridspacing)
	ON_COMMAND(ID_TOOLS_EVENTSCOPYTOCLIPBOARD, OnToolsEventscopytoclipboard)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EVENTSCOPYTOCLIPBOARD, OnUpdateToolsEventscopytoclipboard)
	ON_COMMAND(ID_TOOLS_EVENTSWRITETOLISPFILE, OnToolsEventswritetolispfile)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EVENTSWRITETOLISPFILE, OnUpdateToolsEventswritetolispfile)
END_MESSAGE_MAP()


// COpenDCLApp construction

COpenDCLApp::COpenDCLApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

FontSettings COpenDCLApp::GetDefaultFontSettings() const
{
	CWinApp* pApp = AfxGetApp();
	static const CString sSection = theWorkspace.GetAppKey();
	FontSettings FS( pApp->GetProfileString( sSection, S_DefaultFontName, NULL ),
									 (long)pApp->GetProfileInt( sSection, S_DefaultFontSize, -10 ),
									 (0 != pApp->GetProfileInt( sSection, S_DefaultFontBold, 0 )),
									 (0 != pApp->GetProfileInt( sSection, S_DefaultFontUnderLine, 0 )),
									 (0 != pApp->GetProfileInt( sSection, S_DefaultFontItalic, 0 )) );
	if( !FS )
		FS.setName( theWorkspace.GetDefaultFontName() );
	if( pApp->GetProfileInt( sSection, S_DefaultFontSizeStyle, 0 ) != 0 )
	{
		if( !FS.isScaled() )
			FS.setSize( -FS.size() ); //if "size style" is non-zero, make the size positive to indicate "point size"
	}
	return FS;
}

void COpenDCLApp::SetDefaultFontSettings( const FontSettings& FS, UINT flags /*= fontAll*/ )
{
	if( !FS )
		return;
	CWinApp* pApp = AfxGetApp();
	const CString sSection = theWorkspace.GetAppKey();
	if( flags & fontName )
		pApp->WriteProfileString( sSection, S_DefaultFontName, FS.name() );
	if( flags & fontSize )
		pApp->WriteProfileInt( sSection, S_DefaultFontSize, FS.size() );
	else if( flags & fontScaled )
	{
		long lSize = pApp->GetProfileInt( sSection, S_DefaultFontSize, -10 );
		if( (lSize > 0) ^ FS.isScaled() )
			pApp->WriteProfileInt( sSection, S_DefaultFontSize, -lSize );
	}
	if( flags & fontBold )
		pApp->WriteProfileInt( sSection, S_DefaultFontBold, FS.isBold() );
	if( flags & fontUnderlined )
		pApp->WriteProfileInt( sSection, S_DefaultFontUnderLine, FS.isUnderlined() );
	if( flags & fontItalic )
		pApp->WriteProfileInt( sSection, S_DefaultFontItalic, FS.isItalic() );
}

UINT COpenDCLApp::GetGridSpacing() const
{
	return const_cast< COpenDCLApp* >( this )->GetProfileInt( theWorkspace.GetAppKey(), _T("GridSpacing"), 8 );
}

void COpenDCLApp::SetGridSpacing( UINT nGridSpacing )
{
	WriteProfileInt( theWorkspace.GetAppKey(), _T("GridSpacing"), nGridSpacing );
	theStudioWorkspace.OnGridSpacingChange( nGridSpacing );
}


// The one and only COpenDCLApp object

COpenDCLApp theApp;


// COpenDCLApp initialization

BOOL COpenDCLApp::InitInstance()
{
	AfxSetResourceHandle( theWorkspace.GetLocalResourceModule() );
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	EnableHtmlHelp();
	delete [] m_pszHelpFilePath;
	m_pszHelpFilePath = new TCHAR[MAX_PATH];
	lstrcpyn( (LPTSTR)m_pszHelpFilePath, theWorkspace.FindFile( _T("OpenDCL.chm") ), MAX_PATH );

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("OpenDCL"));
	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(COpenDCLDoc),
		RUNTIME_CLASS(CStudioFrame),       // main SDI frame window
		RUNTIME_CLASS(CProjectPane));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
	CHtmlBrowser mBrowser;

public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LICENSETXT, mBrowser);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
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

	CString sLicenseFile = theWorkspace.GetLanguageSubfolderPath() + _T("License.htm");
	try
	{
		CString sHtml;
		CStdioUnicodeFile File( sLicenseFile, CFile::modeRead | CFile::shareDenyNone );
		CString sLine;
		while( File.ReadString( sLine ) )
			sHtml += sLine;
		mBrowser.LoadHtmlCode( sHtml );
	}
	catch( CFileException* e )
	{
		e->ReportError();
		e->Delete();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// COpenDCLApp message handlers

BOOL COpenDCLApp::OnOpenRecentFile(UINT nID)
{
	//if( !SaveAllModified() )
	//	return FALSE;
	return CWinApp::OnOpenRecentFile(nID);
}

void COpenDCLApp::OnFileNew() 
{
	//if( !SaveAllModified() )
	//	return;
	CWinApp::OnFileNew();
}

void COpenDCLApp::OnFileOpen() 
{
	//if( !SaveAllModified() )
	//	return;
	CWinApp::OnFileOpen();
}

void COpenDCLApp::OnFileClose() 
{
	//if( !SaveAllModified() )
	//	return;
}

void COpenDCLApp::OnHelp() 
{
	HtmlHelp(0, HH_DISPLAY_TOPIC);
}

void COpenDCLApp::OnHelpFinder() 
{
}

void COpenDCLApp::OnToolsDefaultfont() 
{
	CPropertySheet Dlg;
	CFontPropertyPage FontPage;
	Dlg.AddPage(&FontPage);
	Dlg.SetTitle(theWorkspace.LoadResourceString(IDS_SETDEFAULTFONT));
	Dlg.DoModal();
}

void COpenDCLApp::OnToolsGridspacing() 
{
	CGridSpacingDlg Dlg;
	Dlg.DoModal();
}

void COpenDCLApp::OnToolsEventscopytoclipboard() 
{
	CString sSection = theWorkspace.GetAppKey();
	BOOL bNewVal = !AfxGetApp()->GetProfileInt(sSection, _T("EventsCopyToClipboard"), TRUE);
	AfxGetApp()->WriteProfileInt(sSection, _T("EventsCopyToClipboard"), bNewVal);
	theStudioWorkspace.GetPropertyPane()->m_EventsTabPane.GetDlgItem(IDC_COPYTOCLIPBOARD)->ShowWindow(bNewVal? SW_SHOW : SW_HIDE);	
}

void COpenDCLApp::OnUpdateToolsEventscopytoclipboard(CCmdUI* pCmdUI) 
{
	BOOL bChecked =
		AfxGetApp()->GetProfileInt(theWorkspace.GetAppKey(), _T("EventsCopyToClipboard"), TRUE);
	pCmdUI->SetCheck(bChecked);	
}

void COpenDCLApp::OnToolsEventswritetolispfile() 
{
	CString sSection = theWorkspace.GetAppKey();
	BOOL bNewVal = !AfxGetApp()->GetProfileInt(sSection, _T("EventsWriteToLispFile"), FALSE);
	AfxGetApp()->WriteProfileInt(sSection, _T("EventsWriteToLispFile"), bNewVal);
	theStudioWorkspace.GetPropertyPane()->m_EventsTabPane.GetDlgItem(IDC_ADDTOLISP)->ShowWindow(bNewVal? SW_SHOW : SW_HIDE);	
}

void COpenDCLApp::OnUpdateToolsEventswritetolispfile(CCmdUI* pCmdUI) 
{
	BOOL bChecked =
		AfxGetApp()->GetProfileInt(theWorkspace.GetAppKey(), _T("EventsWriteToLispFile"), FALSE);
	pCmdUI->SetCheck(bChecked);	
}
