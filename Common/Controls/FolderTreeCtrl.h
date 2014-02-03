// FolderTreeCtrl.h : header file
//

#pragma once

class CFolderComboBox;


/////////////////////////////////////////////////////////////////////////////
// CFolderTreeCtrl window

class CFolderTreeCtrl : public CTreeCtrl
{
	CImageList mImageList;
	HTREEITEM mhtiSelected;
	CString msAutoSearch;
	DWORD mdwLastCharTime;
	CFolderComboBox* mpFolderCombo;
	bool mbTracking;
	CWnd* mpActiveWnd;

	// Construction
public:
	CFolderTreeCtrl();
	virtual ~CFolderTreeCtrl();

public:
	virtual bool Create( CFolderComboBox* pFolderCombo, const CRect& rectWnd, DWORD dwStyle, UINT nID );

// Operations
protected:
	HTREEITEM AddMyComputer();
	HTREEITEM AddDesktop();
	HRESULT AddSubfolders( IShellFolder* pSfRoot, IShellFolder* pSfParent, HTREEITEM htiParent );
	HTREEITEM SearchPath( HTREEITEM hItem, LPCTSTR pszPath, bool bExactMatch = true ) const;
	HTREEITEM SearchChildOneLevel( HTREEITEM hItem, LPCTSTR pszPath ) const;
	HTREEITEM AddFolder( LPCTSTR pszDisplayName, LPCTSTR pszPath, int nImg, int nImgExpanded, HTREEITEM htiParent );

// Implementation
public:
	void Open();
	void EnableTracking( bool bEnable = true ) { mbTracking = bEnable; }
	void CommitCurrentItem();
	void NotifySelChange() const;
	CString GetSelectedDisplayName() const;
	CString GetItemDisplayName(HTREEITEM item) const;
	CString GetItemPath(HTREEITEM item) const;
	int GetItemImageIndex(HTREEITEM item) const;
	CString GetSelectedPath() const;
	bool SelectPath( LPCTSTR pszPath );
	HTREEITEM AddPath( LPCTSTR pszPath );
	BOOL SelectItem(HTREEITEM item);
	HTREEITEM GetSelectedItem() const;

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnAcadKeepFocus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg BOOL OnNcActivate(BOOL bActive);
};
