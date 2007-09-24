// ProjectTreeCtrl.h : header file
//

#pragma once

#include "EditorProject.h"

class COpenDCLDoc;


/////////////////////////////////////////////////////////////////////////////
// CProjectTreeCtrl window

class CProjectTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CProjectTreeCtrl();

// Attributes
protected:
	TEditorProjectPtr mpProject;
	COpenDCLDoc* mpDocument;
	HTREEITEM mhtiModalParent;
	HTREEITEM mhtiModelessParent;
	HTREEITEM mhtiDockableParent;
	HTREEITEM mhtiConfigParent;	
	HTREEITEM mhtiFileDialogParent;
	HTREEITEM mhtiAutoLispFileParent;
	HTREEITEM mhtiAutoLispFile;
	HTREEITEM mhtiPasswordParent;
	HTREEITEM mhtiPassword;
	HTREEITEM mhtiAxFilesParent;

public:
	HTREEITEM GetModalParentTreeItem() const { return mhtiModalParent; }
	HTREEITEM GetModelessParentTreeItem() const { return mhtiModelessParent; }
	HTREEITEM GetDockableParentTreeItem() const { return mhtiDockableParent; }
	HTREEITEM GetConfigParentTreeItem() const { return mhtiConfigParent; }
	HTREEITEM GetFileDialogParentTreeItem() const { return mhtiFileDialogParent; }
	HTREEITEM GetAutoLispFileParentTreeItem() const { return mhtiAutoLispFileParent; }
	HTREEITEM GetAutoLispFileTreeItem() const { return mhtiAutoLispFile; }
	HTREEITEM GetPasswordParentTreeItem() const { return mhtiPasswordParent; }
	HTREEITEM GetPasswordTreeItem() const { return mhtiPassword; }
	HTREEITEM GetAxFilesParentTreeItem() const { return mhtiAxFilesParent; }
	void SetAutoLispFilename( LPCTSTR pszLispFilename );
	void SetPassword( LPCTSTR pszPassword );
	void SetupProjectTree( TEditorProjectPtr pProject = NULL );
	TEditorProjectPtr GetProject() const { return mpProject; }
	COpenDCLDoc* GetDocument() const { return mpDocument; }
	void SetDocument(COpenDCLDoc* pDocument) { mpDocument = pDocument; }

// Operations
public:
	void ClearTree();
	HTREEITEM FindTabParent(TDclFormPtr pDclTab);
	void RemoveViewPointer(CView *pView);
	void AddFormToTree(TDclFormPtr pDcl, bool bForceShow);
	void RemoveChildren(HTREEITEM hParent);
	void CleanupParents();
	void AddActiveXFileTree(CString sFileName);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectTreeCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProjectTreeCtrl();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CProjectTreeCtrl)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
