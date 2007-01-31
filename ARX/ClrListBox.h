// ClrListBox.h : header file
//

#pragma once

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CClrListBox window

class CClrListBox : public CAcUiListBox
{
// Construction
public:
	CClrListBox();

// Attributes
public:
	CBrush *m_pStaticBrush;		
	COLORREF m_ForeColor;
	COLORREF m_BackColor;
	CDclControlObject *m_ArxControl;

// Operations
public:
	void SetAcadColor(long nColor);
	void SetForeColor(long nColor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClrListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CClrListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CClrListBox)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
