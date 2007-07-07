// FontSizes.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CFontSizes window

class CFontSizes : public CComboBox
{
// Construction
public:
	CFontSizes();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontSizes)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFontSizes();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFontSizes)
	afx_msg void OnSelchange();
	afx_msg void OnEditchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
