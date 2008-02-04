// ZOrderPane.h : header file
//

#pragma once

#include "ZOrderListBox.h"
#include "PtrTypes.h"

class CStudioDialogObject;


#define ZOrderListID  107
#define SendToBackId  108
#define BringToFrontId  109
#define ZUpArrowId  110
#define ZDownArrowId  111
#define ID_TOOLBAR  14
#define nDeButtonCountZO  4
#define nDeToolTipTitleLen  79

#define nDeFontPtSize 80
#define nDeIconW 19
#define nDeIconHt 16


/////////////////////////////////////////////////////////////////////////////
// CZOrderPane window

class CZOrderPane : public CDialog
{
	CZOrderListBox mZOrderList;
	CToolBarCtrl	m_Buttons;
	TBBUTTON		*m_pTBButtons;

// Construction
public:
	CZOrderPane();
	virtual ~CZOrderPane();

// Attributes
public:	

// Operations
public:
	void OnActivateDlgObject( CStudioDialogObject* pDlgObject ) { mZOrderList.OnActivateDlgObject( pDlgObject ); }
	void OnActivateDclControl( TDclControlPtr pDclControl ) { mZOrderList.OnActivateDclControl( pDclControl ); }
	void AddTheButtons();
	CString NeedText( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	BOOL OnNeedText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	void OnNeedTextA( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
	void OnNeedTextW( UINT nID, NMHDR * pNotifyStruct, LRESULT * lResult );
};
