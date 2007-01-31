// ComboBoxHolder.h : header file
//

#pragma once

#define IDC_CB_CHILD 100


/////////////////////////////////////////////////////////////////////////////
// CComboBoxHolder window

class CComboBoxHolder : public CStatic
{
// Construction
public:
	CComboBoxHolder();

// Attributes
public:
	CWnd *pComboBox;
	int m_nStyle;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxHolder)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboBoxHolder();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxHolder)
	afx_msg void OnKillfocusCombo();
	afx_msg void OnSelchangeCombo();	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
