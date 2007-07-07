// ListHeader.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CListHeader window

class CListHeader : public CHeaderCtrl
{
// Construction
public:
	CListHeader();

// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListHeader)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListHeader();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListHeader)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
