// 3DRect.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// C3DRect window

class C3DRect : public CWnd
{
// Construction
public:
	C3DRect();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DRect)
	public:
	virtual BOOL Create(int nStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~C3DRect();

	// Generated message map functions
protected:
	//{{AFX_MSG(C3DRect)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_ClassName;
	int m_BrdStyle;
};
