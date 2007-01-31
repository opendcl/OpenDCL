// Project.h : header file
//

#pragma once

#include "scbarg.h"


/////////////////////////////////////////////////////////////////////////////
// CProjectBar window

#ifndef baseCProjectBar
#define baseCProjectBar CSizingControlBarG
#endif

class CProjectBar : public baseCProjectBar
{
// Construction
public:
	CProjectBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProjectBar();

protected:
	CEdit	m_wndChild;
	CFont	m_font;

	// Generated message map functions
protected:
	//{{AFX_MSG(CProjectBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
