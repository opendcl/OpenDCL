// ProjectTreeCtrl.h : header file
//

#pragma once

class CProject;
class CObjectDCLDoc;


/////////////////////////////////////////////////////////////////////////////
// CProjectTreeCtrl window

class CProjectTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CProjectTreeCtrl();

// Attributes
protected:
	CProject* mpProject;
	CObjectDCLDoc* mpDocument;
	HTREEITEM mhtiModalParent;
	HTREEITEM mhtiModelessParent;
	HTREEITEM mhtiDockableParent;
	HTREEITEM mhtiConfigParent;	
	HTREEITEM mhtiFileDialogParent;
	HTREEITEM mhtiAutoLispFileParent;
	HTREEITEM mhtiAutoLispFile;
	HTREEITEM mhtiOdsFileParent;
	HTREEITEM mhtiOdsFile;
	HTREEITEM mhtiAxFilesParent;

public:
	HTREEITEM GetModalParentTreeItem() const { return mhtiModalParent; }
	HTREEITEM GetModelessParentTreeItem() const { return mhtiModelessParent; }
	HTREEITEM GetDockableParentTreeItem() const { return mhtiDockableParent; }
	HTREEITEM GetConfigParentTreeItem() const { return mhtiConfigParent; }
	HTREEITEM GetFileDialogParentTreeItem() const { return mhtiFileDialogParent; }
	HTREEITEM GetAutoLispFileParentTreeItem() const { return mhtiAutoLispFileParent; }
	HTREEITEM GetAutoLispFileTreeItem() const { return mhtiAutoLispFile; }
	HTREEITEM GetOdsFileParentTreeItem() const { return mhtiOdsFileParent; }
	HTREEITEM GetOdsFileTreeItem() const { return mhtiOdsFile; }
	HTREEITEM GetAxFilesParentTreeItem() const { return mhtiAxFilesParent; }
	void SetAutoLispFilename( LPCTSTR pszLispFilename ) { SetItemText(mhtiAutoLispFile, pszLispFilename); }
	void SetupProjectTree( CProject* pProject = NULL );
	CProject* GetProject() const { return mpProject; }
	CObjectDCLDoc* GetDocument() const { return mpDocument; }
	void SetDocument(CObjectDCLDoc* pDocument) { mpDocument = pDocument; }

// Operations
public:
	void ClearTree();
	HTREEITEM FindTabParent(class CDclFormObject *pDclTab);
	void RemoveViewPointer(CView *pView);
	void AddFormToTree(CDclFormObject *pDcl, bool bForceShow);
	void DeleteChildTab(HTREEITEM hChild);
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
