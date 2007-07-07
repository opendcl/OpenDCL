// WinColorDlg.h : header file
//

#pragma once

#include "Resource.h"
#include "ColorListBox.h"


/////////////////////////////////////////////////////////////////////////////
// CWinColorDlg dialog

class CWinColorDlg : public CDialog
{
// Construction
public:
	CWinColorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWinColorDlg)
	enum { IDD = IDD_WINCOLORDIALOG };
	CButton	m_Cancel;
	CColorListBox	m_WinColorListBox;
	int m_nColorSelection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWinColorDlg)
	afx_msg void OnSelchangeList();
	afx_msg void OnDblclkList();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
