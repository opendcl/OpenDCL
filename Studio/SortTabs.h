// SortTabs.h : header file
//

#pragma once

#include "Resource.h"
#include "TabsPane.h"
#include "DclControlTemplate.h"

class CDclFormObject;
class COpenDCLView;


/////////////////////////////////////////////////////////////////////////////
// CSortTabs dialog

class CSortTabs : public CPropertyPage
{
	TDclControlPtr mpDclControl;
	CTabsPane* mpTabsPane;
	CListBox	m_List;

// Dialog Data
	enum { IDD = IDD_SORTTABS };

// Construction
public:
	CSortTabs( TDclControlPtr pControl, CTabsPane* pTabsPane );
	~CSortTabs();

// Overrides
public:
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	DECLARE_MESSAGE_MAP()
};
