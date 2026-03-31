// GridSpacingDlg.h : header file
//

#pragma once

#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CGridSpacingDlg dialog

class CGridSpacingDlg : public CDialog
{
	CString sCurrentSettingFmt;

// Construction
public:
	CGridSpacingDlg();   // standard constructor

// Dialog Data
	enum { IDD = IDD_GRIDSPACING };
	CSliderCtrl	m_Slider;
	CStatic	m_CurrentSpacing;

	int m_GridSpacing;
	void SetDispaySetting();

// Overrides
protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

// Implementation
	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override;
	void OnOK() override;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
