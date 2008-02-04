// StudioFrame.h : interface of the CStudioFrame class
//


#pragma once

#include "SCBarG.h"
#include "ProjectPane.h"
#include "ToolboxPane.h"
#include "ZOrderPane.h"
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
	CZOrderPane& GetZOrderPane() { return m_wndZOrderPane; }
	CPropertyPane& GetPropertyPane() { return m_wndPropertyPane; }

// Operations
public:
	void OnFontChange( const FontSettings& FS ); // change font

	CDclFormView* OpenDclFormView( TDclFormPtr pDclForm );
	TDclFormPtr AddNewDclFormView( DclFormType nType );

protected:
	BOOL VerifyBarState( LPCTSTR lpszProfileName );

// Implementation
public:
	virtual ~CStudioFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
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
	
	CSizingControlBarG m_wndZOrderSizingBar; // ZOrder bar
	CZOrderPane m_wndZOrderPane;

	CStatusBar m_wndStatusBar;

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

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


