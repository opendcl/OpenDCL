// Help1.h : header file
//

#pragma once

#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CHelp1 dialog

class CHelp1 : public CDialog
{
// Construction
public:
	CHelp1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHelp1)
	enum { IDD = IDD_HELP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int m_nType;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHelp1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHelp1)
	afx_msg void OnCtrls();
	afx_msg void OnGeneral();
	virtual BOOL OnInitDialog();
	afx_msg void OnGeneral2();
	afx_msg void OnBonus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
