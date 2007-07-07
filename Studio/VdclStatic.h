// VdclStatic.h : header file
//

#pragma once

#define nButtonFace -16
#define nButtonText -19


/////////////////////////////////////////////////////////////////////////////
// VdclStatic window

class VdclStatic : public CStatic
{
// Construction
public:
	VdclStatic();
	
// Attributes
public:
	CBrush *m_pStaticBrush;	
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
