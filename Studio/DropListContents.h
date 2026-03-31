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
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDropListContents)
	void OnOK() override;
	BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
