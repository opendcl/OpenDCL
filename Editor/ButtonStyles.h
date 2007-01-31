// ButtonStyles.h : header file
//

#pragma once

#include "Resource.h"
#include "PictureBox.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CButtonStyles dialog

class CButtonStyles : public CPropertyPage
{
	DECLARE_DYNCREATE(CButtonStyles)

// Construction
public:
	CButtonStyles();
	~CButtonStyles();

// Dialog Data
	//{{AFX_DATA(CButtonStyles)
	enum { IDD = IDD_BUTTONSTYLES };
	CPictureBox	m_Picture;
	CListBox	m_PicList;
	CStatic	m_Desc;
	//}}AFX_DATA

	int m_nHighestId;
	int m_SelectedPic;
	int m_SelectedStyle;
	CPropertyObject *m_pStyle;
	CPropertyObject *m_pPicture;
	void DisplayDesc(int nSetting);


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CButtonStyles)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CButtonStyles)
	afx_msg void OnStyle0();
	afx_msg void OnStyle1();
	afx_msg void OnStyle2();
	afx_msg void OnStyle3();
	afx_msg void OnStyle4();
	afx_msg void OnSelchangePiclist();
	afx_msg void OnStyle5();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
