// Splitter.h : header file
//

#pragma once

#define Splitter_Raised 0
#define Splitter_DoubleRaised 1
#define Splitter_Sunken 2


/////////////////////////////////////////////////////////////////////////////
// CSplitter window

class CSplitter : public CStatic
{
// Construction
public:
	CSplitter();

// Attributes
public:

// Operations
public:
	int m_nStyle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitter)
	public:
	virtual BOOL Create(int nStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplitter();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSplitter)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_BrdStyle;
};
