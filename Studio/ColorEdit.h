// ColorEdit.h : header file
//

#pragma once

#define nButtonFace -16
#define nButtonText -19


/////////////////////////////////////////////////////////////////////////////
// CColorEdit window

class CColorEdit : public CEdit
{
// Construction
public:
	CColorEdit();

// Attributes
public:
	CBrush *m_pStaticBrush;		
	COLORREF m_ForeColor;
	COLORREF m_BackColor;
	bool m_UseBackColor;
// Operations
public:
	void SetAcadColor(long nColorArg);
	void SetForeColor(long nColorArg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
