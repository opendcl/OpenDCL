// ColorPatch.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CColorPatch window

class CColorPatch : public CStatic
{
// Construction
public:
	CColorPatch();

// Attributes
public:
	COLORREF m_color;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPatch)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorPatch();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorPatch)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
