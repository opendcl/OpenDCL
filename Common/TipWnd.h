// TipWnd.h : header file
//

#pragma once

//////////////////////////////////////////////////////////////////////////
// © Paramax Technology Limited                                         // 
// ----------------------------                                         //
//                                                                      //
// The author accepts no liablility for injury or loss of profits       // 
// if this software is used. You willingness to use this software       //
// indicates you accept total liability                                 //
//                                                                      // 
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CTipWnd window

class CTipWnd : public CWnd
{
// Construction
public:
	CTipWnd();

// Attributes
public:
protected:
	CFont		m_font;
	CString		m_strFont;
	
// Operations
public:
	BOOL Create(CWnd* pParent);
	void ShowTips(CPoint pt, const CString& str);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTipWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTipWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTipWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
