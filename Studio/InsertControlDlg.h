// InsertControlDlg.h : header file
//

#pragma once

#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CInsertControlDlg dialog

class CInsertControlDlg : public CDialog
{
// Construction
public:
	CInsertControlDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInsertControlDlg)
	enum { IDD = IDD_INSERTCONTROL };
	CStatic m_staticServerPath;
	CButton m_butOK;
	CListBox    m_lbControls;
	//}}AFX_DATA

	CLSID m_clsid;
	CString m_FileName;
	CString m_License;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsertControlDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	 void RefreshControlList();

	 ICatInformationPtr m_pCatInfo;
	 CArray< CATID, CATID& > m_aImplementedCategories;
	 CArray< CATID, CATID& > m_aRequiredCategories;
	 CList< CLSID, CLSID& > m_lControls;

	// Generated message map functions
	//{{AFX_MSG(CInsertControlDlg)
	BOOL OnInitDialog() override;
	afx_msg void OnControlsDblClk();
	afx_msg void OnControlsSelChange();
	afx_msg void OnRegister();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
