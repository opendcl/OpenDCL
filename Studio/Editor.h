// Editor.h : main header file for the OBJECTDCL application
//

#pragma once


#include "MainFrm.h"       // main symbols
#include "OpenDCLView.h" 

class CProject;
class CEditorWorkspace;
class CProjectCollection;
class CHelp2;
enum DclFormType;


#define TCLOG_NULL  0
#define TCLOG_OUTPUTWINDOW  1
#define TCLOG_DEBUG  2
#define TCLOG_FILE  3

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
// COpenDCLApp:
// See OpenDCL.cpp for the implementation of this class
//

class COpenDCLApp : public CWinApp
{
public:
	COpenDCLApp();
	CMainFrame*			m_pMainFrame;

	// ActiveX handling options
	CTCOptions			m_options;
	CHelp2				*m_pCtrlHelp;

// operations
public:
	COpenDCLView* OpenExistingForm(TDclFormPtr pDclForm);
	TDclFormPtr AddNewDclForm(DclFormType nType);
	COpenDCLView* AddDclFormAndView(DclFormType nType);
	CString CreateUniqueName();
	
	CMainFrame* GetMainFrame() { return (CMainFrame*)m_pMainWnd; }

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	CMDIChildWnd*		CreateOrActivateFrame(CDocument* pDoc, CSize ViewSize, bool bResizable);

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg BOOL OnOpenRecentFile(UINT nID);
	afx_msg void OnToolsDefaultfont();
	afx_msg void OnFileClose();
	afx_msg void OnToolsGridspacing();
	afx_msg void OnUpdateToolsEventscopytoclipboard(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsEventswritetolispfile(CCmdUI* pCmdUI);
	afx_msg void OnToolsEventscopytoclipboard();
	afx_msg void OnToolsEventswritetolispfile();
	afx_msg void OnHelp();
	afx_msg void OnHelpFinder();
};
