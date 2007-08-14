// ClrListBox.h : header file
//

#pragma once

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CClrListBox window

class CClrListBox : public CAcUiListBox
{
	CBrush mbrushBackground;

// Construction
public:
	CClrListBox();
	virtual ~CClrListBox();

// Attributes
public:
	COLORREF m_ForeColor;
	COLORREF m_BackColor;
	CDclControlObject *m_ArxControl;

// Operations
public:
	void SetAcadColor(long nColor);
	void SetForeColor(long nColor);

protected:
	DECLARE_MESSAGE_MAP()

	// Generated message map functions
protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};
