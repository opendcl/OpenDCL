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

// Dialog Data
	enum { IDD = IDD_FORMVARNAMEUPDATE };

// Construction
public:
	CFormVarNameUpdate( TDclFormPtr pDclForm, LPCTSTR pszFormName = NULL, CWnd* pParent = NULL );

public:
	INT_PTR DoModal() override;

protected:
	DECLARE_MESSAGE_MAP()

	void OnOK() override;
	BOOL OnInitDialog() override;
	afx_msg void OnCtrlcheck();
};
