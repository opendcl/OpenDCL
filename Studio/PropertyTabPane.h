// PropertyTabPane.h : header file
//

#pragma once

#include "EventsTabPane.h"
#include "PropertiesTabPane.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertyTabPane window

class CPropertyTabPane : public CDialog
{
// Construction
public:
	CPropertyTabPane();

// Attributes
public:
	CTabCtrl			m_TabCtrl;
	CPropertiesTabPane	m_PropertiesTabPane;
	CEventsTabPane		m_EventsTabPane;
	CFont				m_font;
// Operations
public:
	void SetupTabs();
	void DisplaySelectedControlProperties(CDclControlObject *pControl,COpenDCLView *pView);
	void ClearControlProperties();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyTabPane)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyTabPane();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyTabPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
