// OpenDCL.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OpenDCL.h"
#include "OdclJson.h"
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

// Single source of truth for the Studio HTML Help file name (FindFile + missing dialog).
static const TCHAR g_szHelpFileName[] = _T("OpenDCL.chm");

namespace {

void AppendFilterPair( CString& filter, OPENFILENAME& ofn, const CString& name, const CString& spec )
{
	filter += name;
	filter += (TCHAR)'\0';
	filter += spec;
	filter += (TCHAR)'\0';
	ofn.nMaxCustFilter++;
}

void CollectFilterExts( const CString& filterExt, CStringArray& exts )
{
	int iStart = 0;
	for( ;; )
	{
		CString ext = filterExt.Tokenize( _T(";"), iStart );
		if( ext.IsEmpty() )
			break;
		if( ext[0] == _T('.') )
			exts.Add( ext );
	}
}

CString CombinedFilterSpec( const CStringArray& exts )
{
	CString spec;
	for( INT_PTR i = 0; i < exts.GetSize(); ++i )
	{
		if( i )
			spec += _T(';');
		spec += _T('*');
		spec += exts[i];
	}
	return spec;
}

CString FilterDisplayBase( const CString& filterName )
{
	CString base = filterName;
	const int paren = base.ReverseFind( _T('(') );
	if( paren > 0 )
		base = base.Left( paren );
	base.TrimRight();
	return base.IsEmpty() ? filterName : base;
}

void AddStripExt( CStringArray& exts, const CString& ext )
{
	const int n = ext.GetLength();
	INT_PTR i = 0;
	for( ; i < exts.GetSize(); ++i )
	{
		if( exts[i].GetLength() < n )
			break;
	}
	exts.InsertAt( i, ext );
}

bool PathEndsWithI( const CString& path, const CString& suffix )
{
	if( path.GetLength() < suffix.GetLength() )
		return false;
	return path.Right( suffix.GetLength() ).CompareNoCase( suffix ) == 0;
}

// IFileDialog treats only the last dotted component as the extension, so
// xxx.odcl.json + default "odcl" becomes xxx.odcl.odcl. Strip known project
// suffixes (longest first, repeatedly) before applying the selected one.
CString StripProjectSuffix( CString name, const CStringArray& stripExts )
{
	bool stripped = true;
	while( stripped )
	{
		stripped = false;
		for( INT_PTR i = 0; i < stripExts.GetSize(); ++i )
		{
			if( PathEndsWithI( name, stripExts[i] ) )
			{
				name = name.Left( name.GetLength() - stripExts[i].GetLength() );
				stripped = true;
				break;
			}
		}
	}
	return name;
}

CString ApplyProjectSuffix( const CString& fileName, const CString& extNoDot,
		const CStringArray& stripExts )
{
	if( extNoDot.IsEmpty() || fileName.IsEmpty() )
		return fileName;
	CString stem = StripProjectSuffix( fileName, stripExts );
	if( stem.IsEmpty() )
		return fileName;
	return stem + _T('.') + extNoDot;
}

class CStudioProjectFileDlg : public CFileDialog
{
public:
	CStudioProjectFileDlg( BOOL bOpen, DWORD dwFlags )
		: CFileDialog( bOpen, NULL, NULL, dwFlags, NULL, NULL, 0, TRUE )
	{
		m_szDefExt[0] = 0;
		m_nLastFilterIndex = 1;
	}

	TCHAR m_szDefExt[32];
	CStringArray m_extByFilter; // nFilterIndex-1; no leading dot; empty = none
	CStringArray m_stripExts;   // leading dots, longest first
	DWORD m_nLastFilterIndex;

	CString FilterExtNoDot() const
	{
		const int idx = static_cast<int>( m_ofn.nFilterIndex ) - 1;
		if( idx >= 0 && idx < m_extByFilter.GetSize() )
			return m_extByFilter[idx];
		return CString();
	}

	void SetFilterDefaultExt( const CString& ext )
	{
		lstrcpyn( m_szDefExt, ext, _countof( m_szDefExt ) );
		m_ofn.lpstrDefExt = m_szDefExt[0] ? m_szDefExt : NULL;
		if( m_pIFileDialog )
		{
			IFileDialog* pfd = static_cast<IFileDialog*>( m_pIFileDialog );
			pfd->SetDefaultExtension( CStringW( m_szDefExt ) );
		}
	}

	void RewriteFileNameForFilter( const CString& extNoDot )
	{
		if( m_bOpenFileDialog || extNoDot.IsEmpty() || m_pIFileDialog == NULL )
			return;
		IFileDialog* pfd = static_cast<IFileDialog*>( m_pIFileDialog );
		LPWSTR pszName = NULL;
		if( FAILED( pfd->GetFileName( &pszName ) ) || pszName == NULL )
			return;
		const CString cur( pszName );
		CoTaskMemFree( pszName );
		const CString next = ApplyProjectSuffix( cur, extNoDot, m_stripExts );
		if( next.IsEmpty() || next.CompareNoCase( cur ) == 0 )
			return;
		pfd->SetFileName( CStringW( next ) );
	}

	void OnTypeChange() override
	{
		const CString ext = FilterExtNoDot();
		SetFilterDefaultExt( ext );
		if( m_ofn.nFilterIndex == m_nLastFilterIndex )
			return;
		m_nLastFilterIndex = m_ofn.nFilterIndex;
		RewriteFileNameForFilter( ext );
	}
};

class CStudioDocManager : public CDocManager
{
public:
	BOOL DoPromptFileName( CString& fileName, UINT nIDSTitle,
			DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate ) override;
	void RegisterShellFileTypes( BOOL bCompat ) override;
};

BOOL CStudioDocManager::DoPromptFileName( CString& fileName, UINT nIDSTitle,
		DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate )
{
	CDocTemplate* pDocTemplate = pTemplate;
	if( pDocTemplate == NULL )
	{
		POSITION pos = GetFirstDocTemplatePosition();
		if( pos != NULL )
			pDocTemplate = GetNextDocTemplate( pos );
	}

	CStudioProjectFileDlg dlgFile( bOpenFileDialog,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | lFlags );

	CString title;
	ENSURE( title.LoadString( nIDSTitle ) );

	CString strFilter;
	CString strFilterName;
	CString strFilterExt;
	CStringArray exts;
	if( pDocTemplate != NULL &&
			pDocTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt ) &&
			!strFilterExt.IsEmpty() &&
			pDocTemplate->GetDocString( strFilterName, CDocTemplate::filterName ) &&
			!strFilterName.IsEmpty() )
	{
		CollectFilterExts( strFilterExt, exts );
		if( exts.GetSize() > 0 )
		{
			for( INT_PTR i = 0; i < exts.GetSize(); ++i )
				AddStripExt( dlgFile.m_stripExts, exts[i] );

			AppendFilterPair( strFilter, dlgFile.m_ofn, strFilterName, CombinedFilterSpec( exts ) );
			dlgFile.m_extByFilter.Add( exts[0].Mid( 1 ) );
			dlgFile.SetFilterDefaultExt( exts[0].Mid( 1 ) );
			dlgFile.m_ofn.nFilterIndex = 1;
			dlgFile.m_nLastFilterIndex = 1;

			if( exts.GetSize() > 1 )
			{
				const CString base = FilterDisplayBase( strFilterName );
				for( INT_PTR i = 0; i < exts.GetSize(); ++i )
				{
					CString name;
					name.Format( _T("%s (*%s)"), static_cast<LPCTSTR>( base ),
							static_cast<LPCTSTR>( exts[i] ) );
					AppendFilterPair( strFilter, dlgFile.m_ofn, name, _T("*") + exts[i] );
					dlgFile.m_extByFilter.Add( exts[i].Mid( 1 ) );
				}
			}
		}
	}

	CString allFilter;
	VERIFY( allFilter.LoadString( AFX_IDS_ALLFILTER ) );
	AppendFilterPair( strFilter, dlgFile.m_ofn, allFilter, _T("*.*") );
	dlgFile.m_extByFilter.Add( CString() );

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer( _MAX_PATH );

	const INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();
	if( nResult == IDOK && !bOpenFileDialog )
	{
		const CString ext = dlgFile.FilterExtNoDot();
		// Combined filter (index 1) must not rewrite an existing .odcl.json on Save.
		if( !ext.IsEmpty() && dlgFile.m_ofn.nFilterIndex >= 2 )
			fileName = ApplyProjectSuffix( fileName, ext, dlgFile.m_stripExts );
	}
	return nResult == IDOK;
}

void CStudioDocManager::RegisterShellFileTypes( BOOL bCompat )
{
	CDocManager::RegisterShellFileTypes( bCompat );

	// MFC writes filterExt as one HKCR key, so ".odcl;.odcl.lsp;.odcl.json"
	// never becomes a real association. Register each token instead.
	POSITION pos = GetFirstDocTemplatePosition();
	while( pos != NULL )
	{
		CDocTemplate* pTemplate = GetNextDocTemplate( pos );
		CString strFileTypeId, strFilterExt;
		if( pTemplate == NULL ||
				!pTemplate->GetDocString( strFileTypeId, CDocTemplate::regFileTypeId ) ||
				strFileTypeId.IsEmpty() ||
				!pTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt ) ||
				strFilterExt.IsEmpty() )
			continue;

		CStringArray exts;
		CollectFilterExts( strFilterExt, exts );
		for( INT_PTR i = 0; i < exts.GetSize(); ++i )
		{
			const CString& ext = exts[i];
			CString strTemp;
			LONG lSize = _MAX_PATH * 2;
			const LONG lResult = AfxRegQueryValue( HKEY_CLASSES_ROOT, ext,
					strTemp.GetBuffer( lSize ), &lSize );
			strTemp.ReleaseBuffer();
			if( lResult != ERROR_SUCCESS || strTemp.IsEmpty() || strTemp == strFileTypeId )
			{
				AfxRegSetValue( HKEY_CLASSES_ROOT, ext, REG_SZ, strFileTypeId,
						static_cast<DWORD>( ( strFileTypeId.GetLength() + 1 ) * sizeof( TCHAR ) ) );
			}
		}
	}
}

} // namespace


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

int COpenDCLApp::ExitInstance()
{
	const int n = CWinApp::ExitInstance();
	if( m_bConvertMode )
		return m_nConvertExit;
	return n;
}

BOOL COpenDCLApp::InitInstance()
{
	CString inPath, outPath;
	if( StudioTryGetConvertJob( inPath, outPath ) )
	{
		m_bConvertMode = true;
		m_nCmdShow = SW_HIDE;
		AfxSetResourceHandle( theWorkspace.GetLocalResourceModule() );
		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(InitCtrls);
		InitCtrls.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx( &InitCtrls );
		if( !AfxOleInit() )
		{
			StudioWriteConsoleMessage( _T("OLE init failed"), true );
			m_nConvertExit = 1;
			return FALSE;
		}
		CString err;
		if( !StudioConvertProject( inPath, outPath, err ) )
		{
			StudioWriteConsoleMessage( err.IsEmpty() ? _T("/out failed") : err, true );
			m_nConvertExit = 1;
			return FALSE;
		}
		StudioWriteConsoleMessage( _T("converted: ") + outPath, false );
		m_nConvertExit = 0;
		return FALSE;
	}

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
	lstrcpyn( (LPTSTR)m_pszHelpFilePath, theWorkspace.FindFile( g_szHelpFileName ), MAX_PATH );

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
	if( m_pDocManager == NULL )
		m_pDocManager = new CStudioDocManager;
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

	if( m_bConvertMode )
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
	BOOL OnInitDialog() override;
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
	// hhctrl.ocx AVs if the help path is empty or missing.
	if( !m_pszHelpFilePath || !*m_pszHelpFilePath ||
			GetFileAttributes( m_pszHelpFilePath ) == INVALID_FILE_ATTRIBUTES )
	{
		CString sMsg;
		sMsg.Format( _T("Help file %s was not found."), g_szHelpFileName );
		AfxMessageBox( sMsg, MB_OK | MB_ICONINFORMATION );
		return;
	}
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
