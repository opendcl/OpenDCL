// FileDlgCtrls.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CFileDlgCtrls window

class CFileDlgCtrls : public CStatic
{
// Construction
public:
	CFileDlgCtrls();

// Attributes
public:
	HBITMAP					m_TopBitmap;			
	HBITMAP					m_BottomBitmap;			
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileDlgCtrls)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileDlgCtrls();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileDlgCtrls)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
