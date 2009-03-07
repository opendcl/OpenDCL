// PropertyPane.h : header file
//

#pragma once

#include "EventsTabPane.h"
#include "PropertiesTabPane.h"
#include "PtrTypes.h"
#include <vector>


/////////////////////////////////////////////////////////////////////////////
// CPropertyPane window

class CPropertyPane : public CDialog
{
	std::vector< TDclControlPtr > mControls;

	CFont				m_font;
	CTabCtrl			m_TabCtrl;

	// Construction
public:
	CPropertyPane();

// Attributes
public:
	CPropertiesTabPane	m_PropertiesTabPane;
	CEventsTabPane		m_EventsTabPane;

// Operations
public:
	void OnActivateDclControl( TDclControlPtr pDclControl );
	void OnFlushUndoGroup() { m_PropertiesTabPane.GetPropertiesCtrl().HideEditControls(); }
	const std::vector< TDclControlPtr >& GetActiveControls() const { return mControls; }

protected:

// Overrides
	// ClassWizard generated virtual function overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CPropertyPane();

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};
