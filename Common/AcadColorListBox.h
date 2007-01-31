// AcadColorListBox.h : header file
//

#pragma once

#define nButtonFace -16
#define nButtonText -19


/////////////////////////////////////////////////////////////////////////////
// CColorListBox window

class CAcadColorListBox : public CListBox
{
// Construction
public:
	CAcadColorListBox();

// Attributes
public:
	CBrush *m_pStaticBrush;		
	COLORREF m_ForeColor;
	COLORREF m_BackColor;

// Operations
public:
	void SetAcadColor(long nColorArg);
	void SetForeColor(long nColorArg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAcadColorListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAcadColorListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAcadColorListBox)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
