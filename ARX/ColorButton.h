// ColorButton.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CClrButton window

class CClrButton : public CButton
{
// Construction
public:
	CClrButton();

// Attributes
public:
	CBrush *m_pStaticBrush;		
	COLORREF m_ForeColor;
	COLORREF m_BackColor;

// Operations
public:
	void SetAcadColor(long nColor);
	void SetForeColor(long nColor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClrButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CClrButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CClrButton)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
