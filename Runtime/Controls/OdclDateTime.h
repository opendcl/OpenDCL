#if !defined(AFX_ODCLDATETIME_H__33C581FD_D807_42C7_B1A3_7D11A20734F2__INCLUDED_)
#define AFX_ODCLDATETIME_H__33C581FD_D807_42C7_B1A3_7D11A20734F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OdclDateTime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OdclDateTime window

class OdclDateTime : public CDateTimeCtrl
{
	CBrush mbrushBackground;

// Construction
public:
	OdclDateTime();

// Attributes
public:
	COLORREF m_ForeColor;
	COLORREF m_BackColor;
	COLORREF m_DefForeColor;
	COLORREF m_DefBackColor;
	bool m_UseBackColor;
	COLORREF m_DefaultBackColor;
	
	CString m_strOldValue;
// Operations
public:
	void SetBkColor(COLORREF lColor);
	void SetForeColor(COLORREF lColor);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OdclDateTime)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~OdclDateTime();

	// Generated message map functions
protected:
	//{{AFX_MSG(OdclDateTime)
	afx_msg void OnWmkeydown(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ODCLDATETIME_H__33C581FD_D807_42C7_B1A3_7D11A20734F2__INCLUDED_)
