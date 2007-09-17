// FolderTreeCtrl.h : header file
//

#pragma once

class CFolderComboBox;

class CFolder  
{
public:
	CFolder() : m_imageIndex( -1 ), m_imageIndexExpanded( -1 ) {}
	CFolder( LPCTSTR pathDescription, LPCTSTR path, int imageIndex = 0, int imageIndexExpanded = 1 )
		: m_imageIndex( imageIndex )
		, m_imageIndexExpanded( imageIndexExpanded )
		, m_pathDescription( pathDescription )
		, m_path( path )
		{}
	virtual ~CFolder() {}

public:
	int m_imageIndex;		
	int m_imageIndexExpanded;		
	CString m_pathDescription;	
	CString m_path;	
};


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

// Implementation
public:
	void SelectNextItem(BOOL selectNext=TRUE);
	void Inform();
	CString GetSelectedPathDisplayName();
	CString GetSelectedPath();
	bool SelectPath( LPCTSTR pszPath );
	bool SelectFolder( LPCTSTR pszFolderName );
	HTREEITEM SearchPath( HTREEITEM hItem, LPCTSTR pszPath );
	HTREEITEM SearchFolder( HTREEITEM hItem, LPCTSTR pszFolderName );
	HTREEITEM SearchChildOneLevel( HTREEITEM hItem, LPCTSTR pszPath );
	HTREEITEM AddFolder(CFolder* folder, HTREEITEM parent=TVI_ROOT, bool bCurrentDir = false);
	void AddPath(CString path);
	int SelectItem(HTREEITEM item);
	HTREEITEM GetSelectedItem();
	void HideAndInform();
	HTREEITEM FreeMemory(HTREEITEM item);
	void FreeMemory();
	void Display( const CRect& rcTree );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDestroy();
};
