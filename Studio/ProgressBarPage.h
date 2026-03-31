// ProgressBarPage.h : header file
//

#pragma once

#include "Resource.h"
#include "ProgressTimeToComplete.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// CProgressBarPage dialog

class CProgressBarPage : public CPropertyPage
{
	TDclControlPtr mpDclControl;
	TProgressTimeToComplete	m_Progress;
	CButton	m_Time;
	CButton	m_Percentage;
	CEdit	m_Second;
	CEdit	m_Seconds;
	CEdit	m_Minute;
	CEdit	m_Minutes;

	enum { IDD = IDD_PROGRESSBAR };

// Construction
public:
	CProgressBarPage( TDclControlPtr pDclControl );

	void ShowEditFrame();

protected:
	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	BOOL OnApply() override;
	BOOL OnInitDialog() override;
	afx_msg void OnPercentage();
	afx_msg void OnTime();
	afx_msg void OnChangeSeconds();
};
