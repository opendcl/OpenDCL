// StudioFrame.h : interface of the CStudioFrame class
//

#pragma once

#include "SCBarG.h"
#include "ProjectPane.h"
#include "ToolboxPane.h"
#include "TabOrderPane.h"
#include "PropertyPane.h"
#include "FontToolbar.h"


class CStudioFrame : public CMDIFrameWnd
{
	DECLARE_DYNCREATE(CStudioFrame)
protected: // create from serialization only
	CStudioFrame();

// Attributes
public:
	CStatusBar& GetStatusBar() { return m_wndStatusBar; }
	CFontToolbar& GetFontToolbar() { return m_wndFontToolBar; }
	CToolboxPane& GetToolboxPane() { return m_wndToolBoxPane; }
	CProjectPane& GetProjectPane() { return *m_pwndProjectPane; }
	CTabOrderPane& GetTabOrderPane() { return m_wndTabOrderPane; }
	CPropertyPane& GetPropertyPane() { return m_wndPropertyPane; }

// Operations
public:
	void OnFontChange( const FontSettings& FS, UINT flags = fontAll ); // change font

	CDclFormView* OpenDclFormView( TDclFormPtr pDclForm );
	TDclFormPtr AddNewDclFormView( FormType nType );

protected:
	BOOL VerifyBarState( LPCTSTR lpszProfileName );

// Implementation
public:
	virtual ~CStudioFrame();
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

protected:  // control bar embedded members
	CToolBar m_wndMainToolBar;
	CFontToolbar m_wndFontToolBar;
	CReBar m_wndReBar;

	CSizingControlBarG m_wndToolBoxSizingBar; // toolbox bar
	CToolboxPane m_wndToolBoxPane;

  CSizingControlBarG m_wndProjectSizingBar; // project tree bar
	CProjectPane* m_pwndProjectPane;
	
	CSizingControlBarG m_wndPropertySizingBar;
	CPropertyPane m_wndPropertyPane;
	
	CSizingControlBarG m_wndTabOrderSizingBar; // TabOrder bar
	CTabOrderPane m_wndTabOrderPane;

	CStatusBar m_wndStatusBar;

// Overrides
public:
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) override;
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnWindowClose();
	afx_msg void OnUpdateWindowClose(CCmdUI *pCmdUI);
	afx_msg void OnWindowCloseAll();
	afx_msg void OnUpdateWindowCloseAll(CCmdUI *pCmdUI);
	afx_msg void OnViewFontToolbar();
	afx_msg void OnUpdateViewFontToolbar(CCmdUI* pCmdUI);
	afx_msg void OnViewProject();
	afx_msg void OnUpdateViewProject(CCmdUI* pCmdUI);
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	afx_msg void OnViewToolbox();
	afx_msg void OnUpdateViewToolbox(CCmdUI* pCmdUI);
	afx_msg void OnViewZorder();
	afx_msg void OnUpdateViewZorder(CCmdUI* pCmdUI);
	afx_msg void OnUndoButtonDropdown();
	afx_msg void OnAddFormButtonDropdown();
};


