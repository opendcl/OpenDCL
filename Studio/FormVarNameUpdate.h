// FormVarNameUpdate.h : header file
//

#pragma once

#include "Resource.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// CFormVarNameUpdate dialog

class CFormVarNameUpdate : public CDialog
{
	TDclFormPtr mpDclForm;
	CString msFormName;
	bool mbSetControls;

// Construction
public:
	CFormVarNameUpdate( TDclFormPtr pDclForm, LPCTSTR pszFormName = NULL, CWnd* pParent = NULL );

// Dialog Data
	enum { IDD = IDD_FORMVARNAMEUPDATE_DIALOG };

// Implementation
protected:
	virtual void OnOK();
	afx_msg void OnCtrlcheck();
	virtual BOOL OnInitDialog();

protected:
	DECLARE_MESSAGE_MAP()
};
