// ComboBoxFolder.h : header file
//

#pragma once

#include "TreeCtrlFolder.h"
#include "PPToolTip.h"

class CDclControlObject;
class CDwgDirList;


/////////////////////////////////////////////////////////////////////////////
// CComboBoxFolder window

class CComboBoxFolder : public CComboBox
{
// Construction
public:
	CComboBoxFolder();

// Attributes
public:
	CPPToolTip		m_ToolTip;
	CDclControlObject	*m_ArxControl;
	bool				m_bInvokeWithSendString;
	CDwgDirList			*m_pDwgList;

	// ListCtrl edit data members
	bool m_bESC;
	virtual BOOL Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID);

// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxFolder)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	protected:
     virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
     virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	 virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	
	BOOL GetDroppedState();
	void SetDroppedHeight(int height);
	void SetDroppedWidth(int width);
	int GetDroppedWidth2();
	int GetDroppedHeight();
	void DisplayTree();
	CString GetSelectedPath();
	void AddPath(CString path);
	void Init(int w=0, int h=0);
	virtual ~CComboBoxFolder();
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	
	// Generated message map functions
protected:
	afx_msg long OnSelectItemChange(WPARAM w, LPARAM l);
	//{{AFX_MSG(CComboBoxFolder)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	int m_iconWidth;
	int m_droppedHeight;
	int m_droppedWidth;
	CTreeCtrlFolder	m_treeCtrl;
};
