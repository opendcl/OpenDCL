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
	INT_PTR DoModal() override;

protected:
	DECLARE_MESSAGE_MAP()

	void OnOK() override;
	BOOL OnInitDialog() override;
	afx_msg void OnCtrlcheck();
};
