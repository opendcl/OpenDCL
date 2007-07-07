// CDateTimeHolder.h : header file
//

#pragma once

#include "OdclDateTime.h"


/////////////////////////////////////////////////////////////////////////////
// CDateTimeHolder window

class CDateTimeHolder : public CStatic
{
// Construction
public:
	CDateTimeHolder();

// Attributes
public:
	OdclDateTime	m_Child;
	bool			m_bChildIsData;
// Operations
public:

	BOOL CreateDate(CRect rc, CWnd* pParentWnd, UINT nID);
	BOOL CreateTime(CRect rc, CWnd* pParentWnd, UINT nID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDateTimeHolder)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDateTimeHolder();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDateTimeHolder)
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
