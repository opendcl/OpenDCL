// ObjectDCL.h : main header file for the OBJECTDCL application
//

#pragma once


#include "MainFrm.h"       // main symbols
#include "ObjectDCLView.h" 

class CProject;
class CEditorWorkspace;
class CProjectCollection;
class CHelp2;
enum DclFormType;


#define TCLOG_NULL  0
#define TCLOG_OUTPUTWINDOW  1
#define TCLOG_DEBUG  2
#define TCLOG_FILE  3

#define nNumberOfPrevFiles 8
#define nNotSet -1
#define nCtlFileWidth 416
#define nCtlFileHeight 258

#define nDeCtlFileWidth 427
#define nDeCtlFileHeight 290

#define nDeModalWidth 350
#define nDeModalHeight 250

#define nDeModelessWidth 250
#define nDeModelessHeight 150

#define nDeDockableWidth 250
#define nDeDockableHeight 450

#define nDeConfigTabWidth 600
#define nDeConfigTabHeight 380

#define nDeMinSize 50
#define nDeMaxSize 1000

#define nDeMinDialogWidth 25
#define nDeMinDialogHeight 50

/////////////////////////////////////////////////////////////////////////////
// CTCOptions:
// This class is used for ActiveX controls handling options
//

class CTCOptions
{
public:
   CTCOptions();

public:
   BOOL m_tUserMode;
   int m_iLogType;
   CString m_strLogFileName;
   CString m_strProgID;
   BOOL m_tAllowWindowless;
   BOOL m_tHonorIgnoreActivateWhenVisible;
   BOOL m_tIOleInPlaceSiteEx;
   BOOL m_tIOleInPlaceSiteWindowless;
   BOOL m_tIAdviseSinkEx;
   BOOL m_tSBindHost;
};


/////////////////////////////////////////////////////////////////////////////
// CObjectDCLApp:
// See ObjectDCL.cpp for the implementation of this class
//

class CObjectDCLApp : public CWinApp
{
public:
	CObjectDCLApp();
	CMainFrame*			m_pMainFrame;
	//CMultiDocTemplate*	m_pDocTemplate;
	//CObjectDCLDoc*		m_pDoc;
	// ActiveX handling options
	CTCOptions			m_options;
	CHelp2				*m_pCtrlHelp;

// operations
public:
	//CDocument* OpenDocumentFile(LPCTSTR pszPathName);
	//BOOL OpenProject(LPCTSTR FileName, CProject *pProject);
	BOOL SaveDistributionFile(CProjectCollection *pProjectHolder);
	CObjectDCLView* OpenExistingForm(CDclFormObject *pDclForm);
	CDclFormObject* AddNewDclForm(DclFormType nType);
	CObjectDCLView* AddDclFormAndView(DclFormType nType);
	CString CreateUniqueName();
	
	CMainFrame* GetMainFrame() { return (CMainFrame*)m_pMainWnd; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectDCLApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//virtual BOOL SaveAllModified();
	BOOL				CloseDocument();
	CMDIChildWnd*		CreateOrActivateFrame(CDocument* pDoc, CSize ViewSize, bool bResizable);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CObjectDCLApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg BOOL OnOpenRecentFile(UINT nID);
	afx_msg void OnFileCreatedistfile();
	afx_msg void OnToolsDefaultfont();
	afx_msg void OnFileClose();
	afx_msg void OnToolsGridspacing();
	afx_msg void OnUpdateToolsEventscopytoclipboard(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsEventswritetolispfile(CCmdUI* pCmdUI);
	afx_msg void OnToolsEventscopytoclipboard();
	afx_msg void OnToolsEventswritetolispfile();
	afx_msg void OnToolsExportlanguageexcelspreadsheet();
	afx_msg void OnHelp();
	afx_msg void OnHelpFinder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
