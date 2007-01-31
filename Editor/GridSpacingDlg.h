// GridSpacingDlg.h : header file
//

#pragma once

#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CGridSpacingDlg dialog

class CGridSpacingDlg : public CDialog
{
// Construction
public:
	CGridSpacingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGridSpacingDlg)
	enum { IDD = IDD_GRIDSPACING_DIALOG };
	CSliderCtrl	m_Slider;
	CStatic	m_CurrentSpacing;
	//}}AFX_DATA

	int m_GridSpacing;
	void SetDispaySetting();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridSpacingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGridSpacingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnReleasedcaptureGridslider(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
