// FolderComboBox.h : header file
//

#pragma once

#include "FolderTreeCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CFolderComboBox window

class CFolderComboBox : public CComboBox
{
	int m_iconWidth;
	int m_droppedHeight;
	int m_droppedWidth;
	CFolderTreeCtrl	m_treeCtrl;

public:
	CFolderComboBox();
	virtual ~CFolderComboBox();

public:
	bool Create( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwStyle, UINT nID );

// Implementation
public:
	BOOL GetDroppedState();
	void SetDroppedHeight(int height);
	void SetDroppedWidth(int width);
	int GetDroppedWidth2();
	int GetDroppedHeight();
	void DisplayTree();
	CString GetSelectedPathDisplayName();
	CString GetSelectedPath();
	bool SelectPath( LPCTSTR pszPath );
	bool SelectFolder( LPCTSTR pszFolderName );
	void AddPath( LPCTSTR pszPath );
	
	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnSelchange( WPARAM wParam, LPARAM lParam );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
};
