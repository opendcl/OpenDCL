// TabOrderPane.h : header file
//

#pragma once

#include "TabOrderListBox.h"
#include "PtrTypes.h"

class CStudioDialogObject;


//#define TabOrderListID  107
//#define SendToBackId  108
//#define BringToFrontId  109
//#define ZUpArrowId  110
//#define ZDownArrowId  111
//#define nDeToolTipTitleLen  79
//
//#define nDeFontPtSize 80
//#define nDeIconW 19
//#define nDeIconHt 16


/////////////////////////////////////////////////////////////////////////////
// CTabOrderPane window

class CTabOrderPane : public CDialog
{
	CTabOrderListBox mTabOrderList;
	CToolBarCtrl mToolbar;

// Construction
public:
	CTabOrderPane();
	virtual ~CTabOrderPane();

// Attributes
public:	

// Operations
public:
	void OnActivateDlgObject( CStudioDialogObject* pDlgObject );
	void OnActivateDclControl( TDclControlPtr pDclControl );

protected:
	void UpdateToolUI();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
protected:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;
	BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	void OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	void OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
};
