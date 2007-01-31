// TextBoxFilters.h : header file
//

#pragma once

class CPropertyObject;

#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CTextBoxFilters dialog

class CTextBoxFilters : public CPropertyPage
{
	DECLARE_DYNCREATE(CTextBoxFilters)

// Construction
public:
	CTextBoxFilters();
	~CTextBoxFilters();

// Dialog Data
	//{{AFX_DATA(CTextBoxFilters)
	enum { IDD = IDD_EDITFILTERS };
	CStatic	m_Desc;
	//}}AFX_DATA

	int m_SelectedStyle;
	CPropertyObject *m_pStyle;
	void DisplayDesc(int nSetting);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTextBoxFilters)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTextBoxFilters)
	virtual BOOL OnInitDialog();
	afx_msg void OnFilter0();
	afx_msg void OnFilter1();
	afx_msg void OnFilter2();
	afx_msg void OnFilter3();
	afx_msg void OnFilter4();
	afx_msg void OnFilter5();
	afx_msg void OnFilter6();
	afx_msg void OnFilter7();
	afx_msg void OnFilter8();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
