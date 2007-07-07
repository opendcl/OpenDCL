// OptionListBox.h : header file
//

#pragma once

#include "ClrListBox.h"
#include "PPToolTip.h"


/////////////////////////////////////////////////////////////////////////////
// COptionListBox window

class COptionListBox : public CClrListBox
{
// Construction
public:
	COptionListBox();

// Attributes
public:
	short m_RowHeight;
	short m_NextHeight;

	//CStringArray m_TttMainList;
	//CArray<int, int> m_TttLineList;
	//CStringArray m_TttTitleList;
	//CArray<int, int> m_TttPictureList;
	//CArray<long, long> m_TttTitleColorList;
	//CStringArray m_TttAviList;
	
	bool m_bInvokeWithSendString;
	CImageList m_ImageList;
	CPPToolTip m_ToolTip;

// Operations
public:
	void SetRowHeight(int nNewHeight);
	void SetDragnDrop(BOOL bRegister);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct, int nHighlight);
	void ResetTooltips();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionListBox)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COptionListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(COptionListBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchange();
	afx_msg void OnDblclk();
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);	
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
