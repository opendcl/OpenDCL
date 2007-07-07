#pragma once

#include "Resource.h"


// CProjectPasswordDlg dialog

class CProjectPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CProjectPasswordDlg)

public:
	CProjectPasswordDlg(LPCTSTR pszPassword, CWnd* pParent = NULL);   // standard constructor
	virtual ~CProjectPasswordDlg();

// Dialog Data
	enum { IDD = IDD_PROJECTPASSWORD };
	CString msPassword;

public:
	LPCTSTR GetPassword() const { return msPassword; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
