// FolderTreeCtrl.h : header file
//

#pragma once

class CFolderComboBox;
class CFolder;


/////////////////////////////////////////////////////////////////////////////
// CFolderTreeCtrl window

class CFolderTreeCtrl : public CTreeCtrl
{
	CImageList mImageList;
	HTREEITEM mhtiSelected;
	CFolderComboBox* mpFolderCombo;

	// Construction
public:
	CFolderTreeCtrl();
	virtual ~CFolderTreeCtrl();

public:
	virtual bool Create( CFolderComboBox* pFolderCombo, const CRect& rectWnd, DWORD dwStyle, UINT nID );
	static const UINT& refWM_SELCHANGE();

// Operations
protected:
	void EnumFolders();
	void AddDrives(HTREEITEM myComputer);
	HTREEITEM FreeMemory(HTREEITEM item);
	void FreeMemory();
	HTREEITEM SearchPath( HTREEITEM hItem, LPCTSTR pszPath );
	HTREEITEM SearchFolder( HTREEITEM hItem, LPCTSTR pszFolderName );
	HTREEITEM SearchChildOneLevel( HTREEITEM hItem, LPCTSTR pszPath );
	HTREEITEM AddFolder(CFolder* folder, HTREEITEM parent=TVI_ROOT, bool bCurrentDir = false);
	void Hide();
	void Inform();

// Implementation
public:
	void Display( const CRect& rcTree );
	void SelectNextItem(BOOL selectNext=TRUE);
	CString GetSelectedDisplayName();
	CString GetItemDisplayName(HTREEITEM item);
	CString GetItemPath(HTREEITEM item);
	int GetItemImageIndex(HTREEITEM item);
	CString GetSelectedPath();
	bool SelectPath( LPCTSTR pszPath );
	bool SelectFolder( LPCTSTR pszFolderName );
	void AddPath( LPCTSTR pszPath );
	BOOL SelectItem(HTREEITEM item);
	HTREEITEM GetSelectedItem();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
};
