// DropListContents.h : header file
//

#pragma once

#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CDropListContents dialog

class CDropListContents : public CDialog
{
// Construction
public:
	CDropListContents(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDropListContents)
	enum { IDD = IDD_LISTCONTENTS };
	CEdit	m_ListEdit;
	//}}AFX_DATA
	CStringArray sStrings;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDropListContents)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDropListContents)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
