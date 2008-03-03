// ProjectPane.h : header file
//

#pragma once

#include "StudioProject.h"

class COpenDCLDoc;


/////////////////////////////////////////////////////////////////////////////
// CProjectPane window

class CProjectPane : public CCtrlView
{
	TStudioProjectPtr mpProject;
	CImageList mTreeImageList;
	HTREEITEM mhtiModalParent;
	HTREEITEM mhtiModelessParent;
	HTREEITEM mhtiDockableParent;
	HTREEITEM mhtiConfigParent;	
	HTREEITEM mhtiFileDialogParent;
	HTREEITEM mhtiPaletteParent;
	HTREEITEM mhtiAutoLispFileParent;
	HTREEITEM mhtiAutoLispFile;
	HTREEITEM mhtiPasswordParent;
	HTREEITEM mhtiPassword;
	HTREEITEM mhtiAxFilesParent;

	DECLARE_DYNCREATE(CProjectPane)
public:
	CProjectPane();
	virtual ~CProjectPane();

// Attributes
public:
	CTreeCtrl& GetTreeCtrl() const { return *(CTreeCtrl*)this; }
	bool IsFormSelected() const;
	HTREEITEM GetModalParentTreeItem() const { return mhtiModalParent; }
	HTREEITEM GetModelessParentTreeItem() const { return mhtiModelessParent; }
	HTREEITEM GetDockableParentTreeItem() const { return mhtiDockableParent; }
	HTREEITEM GetConfigParentTreeItem() const { return mhtiConfigParent; }
	HTREEITEM GetFileDialogParentTreeItem() const { return mhtiFileDialogParent; }
	HTREEITEM GetPaletteParentTreeItem() const { return mhtiPaletteParent; }
	HTREEITEM GetAutoLispFileParentTreeItem() const { return mhtiAutoLispFileParent; }
	HTREEITEM GetAutoLispFileTreeItem() const { return mhtiAutoLispFile; }
	HTREEITEM GetPasswordParentTreeItem() const { return mhtiPasswordParent; }
	HTREEITEM GetPasswordTreeItem() const { return mhtiPassword; }
	HTREEITEM GetAxFilesParentTreeItem() const { return mhtiAxFilesParent; }
	void SetAutoLispFilename( LPCTSTR pszLispFilename );
	void SetPassword( LPCTSTR pszPassword );
	void OnActivateProject( TStudioProjectPtr pProject = NULL );
	void OnActivateForm( TDclFormPtr pForm = NULL );
	TStudioProjectPtr GetProject() const { return mpProject; }

// Operations
public:
	HTREEITEM FindTabParent(TDclFormPtr pDclTab);
	void AddFormToTree(TDclFormPtr pDcl, bool bForceShow);
	void RemoveChildren(HTREEITEM hParent);
	void CleanupParents();
	void AddActiveXFileTree(CString sFileName);

public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	
	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRemoveForm();
	afx_msg void OnUpdateRemoveForm(CCmdUI* pCmdUI);
	afx_msg void OnAddmodal();
	afx_msg void OnUpdateAddmodal(CCmdUI *pCmdUI);
	afx_msg void OnAddmodeless();
	afx_msg void OnUpdateAddmodeless(CCmdUI *pCmdUI);
	afx_msg void OnAdddockable();
	afx_msg void OnUpdateAdddockable(CCmdUI *pCmdUI);
	afx_msg void OnAddconfig();
	afx_msg void OnUpdateAddconfig(CCmdUI *pCmdUI);
	afx_msg void OnAddfiledialogbox();
	afx_msg void OnUpdateAddfiledialogbox(CCmdUI *pCmdUI);
	afx_msg void OnAddpalette();
	afx_msg void OnUpdateAddpalette(CCmdUI *pCmdUI);
	afx_msg void OnPicturefolder();
	afx_msg void OnUpdatePicturefolder(CCmdUI *pCmdUI);
	afx_msg void OnSetautolispfilename();
	afx_msg void OnUpdateSetautolispfilename(CCmdUI *pCmdUI);
	afx_msg void OnSetpassword();
	afx_msg void OnUpdateSetpassword(CCmdUI *pCmdUI);
};
