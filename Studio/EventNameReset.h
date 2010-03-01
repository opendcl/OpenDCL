// EventNameReset.h : header file
//

#pragma once

#include "Resource.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// CEventNameReset dialog

class CEventNameReset : public CDialog
{
	TDclFormPtr mpDclForm;
	bool mbSetControls;

// Dialog Data
	enum { IDD = IDD_FORMEVENTNAMERESET };

// Construction
public:
	CEventNameReset( TDclFormPtr pDclForm, CWnd* pParent = NULL );

public:
	virtual INT_PTR DoModal();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCtrlcheck();
};
