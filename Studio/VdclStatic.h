// VdclStatic.h : header file
//

#pragma once

#define nButtonFace -16
#define nButtonText -19


/////////////////////////////////////////////////////////////////////////////
// VdclStatic window

class VdclStatic : public CStatic
{
	CBrush mbrushBackground;

// Construction
public:
	VdclStatic();
	
// Attributes
public:
	COLORREF m_ForeColor;
// Operations
public:
	void SetAcadColor(long nColorArg);
	void SetForeColor(long nColorArg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VdclStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~VdclStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(VdclStatic)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
