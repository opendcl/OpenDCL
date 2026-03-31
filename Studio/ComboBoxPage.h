// ComboBoxPage.h : header file
//

#pragma once

#include "Resource.h"
#include "ListBoxCtrl.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CComboBoxPage dialog

class CComboBoxPage : public CPropertyPage
{
	TDclControlPtr mpControl;

	CComboBoxEx	m_ComboBoxEx;
	CListBox	m_OptionList;
	CStatic	m_Desc;
	int m_nCtrl;
	int m_SelectedStyle;
	CImageList m_ImageList;

	enum { IDD = IDD_COMBOBOX };

// Construction
public:
	CComboBoxPage( TDclControlPtr pControl );
	~CComboBoxPage();
	void DisplayDesc(int nSetting);

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override;
	BOOL OnApply() override;
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	afx_msg void OnSelchangeOptionlist();
};
