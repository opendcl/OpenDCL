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
	BOOL OnSetActive() override;
	BOOL OnApply() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

// Implementation
protected:
	BOOL OnInitDialog() override;
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	DECLARE_MESSAGE_MAP()
};
