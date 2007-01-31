// EventsCheckTree.h : header file
//

#pragma once

class CObjectDCLView;


/////////////////////////////////////////////////////////////////////////////
// CEventsCheckTree window

class CEventsCheckTree : public CTreeCtrl
{
// Construction
public:
	CEventsCheckTree();

// Attributes
public:
	CObjectDCLView *m_pView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventsCheckTree)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEventsCheckTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEventsCheckTree)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
