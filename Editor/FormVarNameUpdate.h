// FormVarNameUpdate.h : header file
//

#pragma once

#include "Resource.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CFormVarNameUpdate dialog

class CFormVarNameUpdate : public CDialog
{
// Construction
public:
	CFormVarNameUpdate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFormVarNameUpdate)
	enum { IDD = IDD_FORMVARNAMEUPDATE_DIALOG };
	BOOL	m_FormCheckBox;
	BOOL	m_CtrlCheck;
	//}}AFX_DATA
	CDclControlObject *m_pControl;
	CString m_sDclFormName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormVarNameUpdate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormVarNameUpdate)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCtrlcheck();
	afx_msg void OnFormscheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
