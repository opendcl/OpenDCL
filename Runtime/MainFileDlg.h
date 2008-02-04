// MainFileDlg.h : header file
//

#pragma once

#include "DialogObject.h"

class CCustomFileDialog;


/////////////////////////////////////////////////////////////////////////////
// CMainFileDlg dialog

class CMainFileDlg : public CCommonDialog
{
	CCustomFileDialog* mpDlgObject;
	bool mbInitialized;

// Construction
public:
	CMainFileDlg( CCustomFileDialog* pDlgObject, CWnd* pParent = NULL );
	virtual ~CMainFileDlg();

protected:
	DECLARE_MESSAGE_MAP()

	// Generated message map functions
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
