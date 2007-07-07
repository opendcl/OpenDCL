// ProgressBarPage.h : header file
//

#pragma once

#include "Resource.h"
#include "ProgressTimeToComplete.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CProgressBarPage dialog

class CProgressBarPage : public CPropertyPage
{
// Construction
public:
	CProgressBarPage(UINT unID);

// Dialog Data
	//{{AFX_DATA(CProgressBarPage)
	enum { IDD = IDD_PROGRESSBAR };
	TProgressTimeToComplete	m_Progress;
	CButton	m_Time;
	CButton	m_Percentage;
	CEdit	m_Second;
	CEdit	m_Seconds;
	CEdit	m_Minute;
	CEdit	m_Minutes;
	//}}AFX_DATA

	CString			m_sTitle;

	CDclControlObject *m_pArxCtrl;

	void ShowEditFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressBarPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnApply();
	//}}AFX_VIRTUAL



// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressBarPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPercentage();
	afx_msg void OnTime();
	afx_msg void OnChangeSeconds();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
