// TabStripCtrl.h : header file
//

#pragma once

#include "DialogControl.h"

class CDclFormObject;


/////////////////////////////////////////////////////////////////////////////
// CTabStripCtrl window

class CTabStripCtrl : public CTabCtrl, public CDialogControl
{
	CBrush mbrushBackground;

// Construction
public:
	CTabStripCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CTabStripCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual bool OnApplyCaption( TPropertyPtr pProp ) { return true; }
	virtual bool OnApplyToolTip( TPropertyPtr pProp );

// Interface
public:
	CTabCtrl& GetTabCtrl() { return *this; }
	const CTabCtrl& GetTabCtrl() const { return *this; }
	CRect GetUsedArea() const;
	virtual int GetTabItemIndex( size_t nPageIndex ) const { return (int)nPageIndex; }
	virtual void SetupTabs();
	virtual void ResetTooltips();

protected:
	DECLARE_MESSAGE_MAP()

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
protected:
	afx_msg void OnDestroy();
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void PostNcDestroy();
};
