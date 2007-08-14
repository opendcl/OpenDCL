// ColorButton.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CClrButton window

class CClrButton : public CButton
{
	CBrush mbrushBackground;

// Construction
public:
	CClrButton();

// Attributes
public:
	COLORREF m_ForeColor;
	COLORREF m_BackColor;

// Operations
public:
	void SetAcadColor(long nColor);
	void SetForeColor(long nColor);

// Implementation
public:
	virtual ~CClrButton();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};
