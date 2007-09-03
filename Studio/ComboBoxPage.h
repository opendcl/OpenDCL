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
// Construction
public:
	CComboBoxPage();
	~CComboBoxPage();
// Dialog Data
	//{{AFX_DATA(CComboBoxPage)
	enum { IDD = IDD_COMBOBOX };
	CComboBoxEx	m_ComboBoxEx;
	CListBox	m_OptionList;
	CStatic	m_Desc;
	//}}AFX_DATA

	int m_nCtrl;
	int m_SelectedStyle;
	CImageList m_ImageList;
	TPropertyPtr m_pStyle;
	void DisplayDesc(int nSetting);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CComboBoxPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeOptionlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
