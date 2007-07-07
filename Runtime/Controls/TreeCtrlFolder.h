// TreeCtrlFolder.h : header file
//

#pragma once

#include "Folder.h"

class CComboBoxFolder;

#define WM_SELECTITEM_CHANGE	WM_USER + 102


/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlFolder window

class CTreeCtrlFolder : public CTreeCtrl
{
// Construction
public:
	CTreeCtrlFolder();

// Attributes
public:
// Operations
public:
	void EnumFolders();
	void AddDrives(HTREEITEM myComputer);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlFolder)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SelectNextItem(BOOL selectNext=TRUE);
	void Inform();
	void Init(CComboBoxFolder* comboBoxFolder);
	CString GetSelectedPath();
	HTREEITEM SearchChildOneLevel(HTREEITEM item, CString path);
	HTREEITEM Search(HTREEITEM item, CString path);
	HTREEITEM AddFolder(CFolder* folder, HTREEITEM parent=TVI_ROOT, bool bCurrentDir = false);
	void AddPath(CString path);
	int SelectItem(HTREEITEM item);
	HTREEITEM GetSelectedItem();
	void HideAndInform();
	HTREEITEM FreeMemory(HTREEITEM item);
	void FreeMemory();
	void Display(CRect rc);
	void CreateImageList();
	virtual ~CTreeCtrlFolder();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeCtrlFolder)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	HTREEITEM m_selectedItem;
	CImageList	m_imageList;
	CComboBoxFolder* m_comboBoxFolder;
};
