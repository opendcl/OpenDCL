// ComboBoxHolder.h : header file
//

#pragma once

#define IDC_CB_CHILD 100


/////////////////////////////////////////////////////////////////////////////
// CComboBoxHolder window

class CComboBoxHolder : public CStatic
{
// Attributes
public:
	CWnd *pComboBox;
	int m_nStyle;

// Construction
public:
	CComboBoxHolder();
	virtual ~CComboBoxHolder();
};
