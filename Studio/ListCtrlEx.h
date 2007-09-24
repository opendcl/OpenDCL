// ListCtrlEx.h : header file
//

#pragma once

#include "PictureBox.h"
#include "DclControlObject.h"


/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx window

class CListCtrlEx : public CPictureBox
{
// Construction
public:
	CListCtrlEx();

// Attributes
public:
	CHeaderCtrl    m_Child;

// Operations
public:
	void SetupColumns(TDclControlPtr pControl);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlEx)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
