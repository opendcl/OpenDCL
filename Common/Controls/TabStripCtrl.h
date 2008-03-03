// TabStripCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"

#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CTabStripCtrl window

class CTabStripCtrl : public CTabCtrl, public CDialogControl
{
	CAcadColorService mColorService;

// Construction
public:
	CTabStripCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CTabStripCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual void ApplyPosition(); //move control window to new position
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual bool OnApplyCaption( TPropertyPtr pProp ) { return true; }
	virtual bool OnApplyToolTip( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }

// Interface
public:
	CTabCtrl& GetTabCtrl() { return *this; }
	const CTabCtrl& GetTabCtrl() const { return *this; }
	CRect GetUsedArea() const;
	virtual int GetTabItemIndex( size_t nPageIndex ) const { return (int)nPageIndex; }
	virtual void SetupTabs();
	virtual void ResetTooltips();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void PostNcDestroy();
	afx_msg void OnDestroy();
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchange( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
};
