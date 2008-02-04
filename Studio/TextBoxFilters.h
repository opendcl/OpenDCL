// TextBoxFilters.h : header file
//

#pragma once

#include "Resource.h"
#include "PtrTypes.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CTextBoxFilters dialog

class CTextBoxFilters : public CPropertyPage
{
	TDclControlPtr mpDclControl;
	CStatic	m_Desc;
	int m_SelectedStyle;
	TPropertyPtr m_pStyle;

	enum { IDD = IDD_EDITFILTERS };

public:
	CTextBoxFilters( TDclControlPtr pDclControl );
	~CTextBoxFilters();

	void DisplayDesc(int nSetting);

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnFilter0();
	afx_msg void OnFilter1();
	afx_msg void OnFilter2();
	afx_msg void OnFilter3();
	afx_msg void OnFilter4();
	afx_msg void OnFilter5();
	afx_msg void OnFilter6();
	afx_msg void OnFilter7();
	afx_msg void OnFilter8();
};
