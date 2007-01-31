// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once


#include "SCBarG.h"
#include "ProjectTreeCtrl.h"
#include "ToolBox.h"
#include "ZOrderPane.h"
#include "PropertyTabPane.h"
#include "MainFrameToolBar.h"


class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	
// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	CMainFrameToolBar  m_wndDlgBar;
	
  CSizingControlBarG		m_wndProjectTreeBar; // project tree bar
	CImageList			m_TreeImageList; // project tree image list
	CProjectTreeCtrl	m_ProjectTree; // project tree child control
	
	CSizingControlBarG		m_wndZOderBar; // ZOrder bar
	CZOrderPane			m_ZOrderPane;

	CSizingControlBarG		m_wndToolBoxBar; // toolbox bar
	CToolBox			m_ToolBox;
	
	CSizingControlBarG		m_wndPropertyBar; // toolbox bar
	CPropertyTabPane	m_PropertyTabPane;
	
	void CreateProjectDockingBar(CProject* pProject = NULL);
	void CreateToolBoxBar();
	void CreateZOrderDockingBar();
	void CreatePropertyDockingBar();

// Generated message map functions
protected:
	BOOL VerifyBarState(LPCTSTR lpszProfileName);
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPicturefolder();
	afx_msg void OnAddmodal();
	afx_msg void OnAddmodeless();
	afx_msg void OnAddconfig();
	afx_msg void OnAdddockable();
	afx_msg void OnSetautolispfilename();
	afx_msg void OnRemove();
	afx_msg void OnDockProject();
	afx_msg void OnUpdateDockProject(CCmdUI* pCmdUI);
	afx_msg void OnDockProperties();
	afx_msg void OnUpdateDockProperties(CCmdUI* pCmdUI);
	afx_msg void OnDockToolbox();
	afx_msg void OnUpdateDockToolbox(CCmdUI* pCmdUI);
	afx_msg void OnDockZorder();
	afx_msg void OnUpdateDockZorder(CCmdUI* pCmdUI);
	afx_msg void OnSetdistfilename();
	afx_msg void OnProjectsAddfiledialogbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
