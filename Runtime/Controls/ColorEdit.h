// ColorEdit.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CColorEdit window

class CColorEdit : public CAcUiEdit
{
	CBrush mbrushBackground;

// Construction
public:
	CColorEdit();

// Attributes
public:
	COLORREF m_ForeColor;
	COLORREF m_BackColor;
	
// Operations
public:
	void SetAcadColor(long nColor);
	void SetForeColor(long nColor);
	void SetForeColor(COLORREF lColor);
	void SetBkColor(COLORREF lColor);

	
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
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	
};
