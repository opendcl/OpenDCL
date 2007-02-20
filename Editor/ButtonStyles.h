// ButtonStyles.h : header file
//

#pragma once

#include "Resource.h"
#include "PictureBox.h"

class CDclControlObject;
class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CButtonStyles dialog

class CButtonStyles : public CPropertyPage
{
	CDclControlObject* mpControl;
	CPictureBox	m_Picture;
	CListBox	m_PicList;
	CStatic	m_Desc;

	int m_nHighestId;
	int m_SelectedPic;
	int m_SelectedStyle;

public:
	RefCountedPtr< CPropertyObject > m_pStyle;
	RefCountedPtr< CPropertyObject > m_pPicture;

protected:
	enum { IDD = IDD_BUTTONSTYLES };

// Construction
public:
	CButtonStyles( CDclControlObject* pControl );
	~CButtonStyles();

public:
	void DisplayDesc(int nSetting);

// Overrides
public:
	virtual BOOL OnApply();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

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

protected:
	DECLARE_MESSAGE_MAP()
};
