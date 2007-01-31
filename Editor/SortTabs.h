// SortTabs.h : header file
//

#pragma once

#include "Resource.h"
#include "TabsPane.h"

class CDclFormObject;
class CDclControlObject;
class CObjectDCLView;


/////////////////////////////////////////////////////////////////////////////
// CSortTabs dialog

class CSortTabs : public CPropertyPage
{
	DECLARE_DYNCREATE(CSortTabs)

// Construction
public:
	CSortTabs();
	~CSortTabs();

// Dialog Data
	//{{AFX_DATA(CSortTabs)
	enum { IDD = IDD_SORTTABS };
	CListBox	m_List;
	//}}AFX_DATA

	CDclFormObject *m_pDclForm;
	CDclControlObject *m_pControl;
	CTabInfoList m_TabList;
	CTabInfoList m_DeletedTabList;
	CObjectDCLView *m_pView;
	CTabsPane *m_pTabPane;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSortTabs)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSortTabs)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
