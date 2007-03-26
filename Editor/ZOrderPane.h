// ZOrderPane.h : header file
//

#pragma once

#include "ZOrderListCtrl.h"

#define ZOrderListID  107
#define SendToBackId  108
#define BringToFrontId  109
#define ZUpArrowId  110
#define ZDownArrowId  111
#define ID_TOOLBAR  14
#define nDeButtonCountZO  4
#define nDeToolTipTitleLen  79

#define nDeFontPtSize 80
#define nDeColWidth200 200
#define nNotSet -1
#define nDeIconW 19
#define nDeIconHt 16


/////////////////////////////////////////////////////////////////////////////
// CZOrderPane window

class CZOrderPane : public CDialog
{
// Construction
public:
	CZOrderPane();

// Attributes
public:	
	CZOrderListCtrl m_ZOrderList;
	CToolBarCtrl	m_Buttons;
	TBBUTTON		*m_pTBButtons;
	CFont			m_font;
// Operations
public:
	void AddTheButtons();
	CString NeedText( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZOrderPane)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CZOrderPane();

	// Generated message map functions
protected:
	//{{AFX_MSG(CZOrderPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBringtofront();
	afx_msg void OnSendtoback();
	afx_msg void OnMovezbackby1();
	afx_msg void OnMovezfrontby1();
	afx_msg void OnZorderhelp();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	BOOL OnNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	void OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	void OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );

	DECLARE_MESSAGE_MAP()
};
