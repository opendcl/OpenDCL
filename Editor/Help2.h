// Help2.h : header file
//

#pragma once

#include "AutoRichEditCtrl.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CHelp2 dialog

class CHelp2 : public CDialog
{
// Construction
public:
	CHelp2(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHelp2)
	enum { IDD = IDD_HELPCTRLS };
	CAutoRichEditCtrl	m_Text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHelp2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHelp2)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
