// ButtonStyles.h : header file
//

#pragma once

#include "Resource.h"
#include "PictureBox.h"
#include "PtrTypes.h"

class CDclControlTemplate;
class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CButtonStyles dialog

class CButtonStyles : public CPropertyPage
{
	TDclControlPtr mpControl;
	CPictureBox	m_Picture;
	CListBox	m_PicList;
	CStatic	m_Desc;

	UINT m_nHighestId;
	UINT m_SelectedPic;
	int m_SelectedStyle;

public:
	TPropertyPtr m_pStyle;
	TPropertyPtr m_pPicture;

protected:
	enum { IDD = IDD_BUTTONSTYLES };

// Construction
public:
	CButtonStyles( TDclControlPtr pControl );
	virtual ~CButtonStyles();

public:
	void DisplayDesc(int nSetting);

// Overrides
public:
	BOOL OnApply() override;
protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CButtonStyles)
	BOOL OnInitDialog() override;
	afx_msg void OnStyle0();
	afx_msg void OnStyle1();
	afx_msg void OnStyle2();
	afx_msg void OnStyle3();
	afx_msg void OnStyle4();
	afx_msg void OnStyle5();
	afx_msg void OnStyle6();
	afx_msg void OnSelchangePiclist();

protected:
	DECLARE_MESSAGE_MAP()
};
