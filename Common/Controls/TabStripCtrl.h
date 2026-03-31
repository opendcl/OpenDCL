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
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	void ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast ) override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	bool OnApplyCaption( TPropertyPtr pProp ) override { return true; }
	bool OnApplyToolTip( TPropertyPtr pProp ) override;
	void HandleDpiChanged() override; //handle relayed WM_DPICHANGED_AFTERPARENT message
	CAcadColorService* GetColorService() override { return &mColorService; }

// Interface
public:
	CTabCtrl& GetTabCtrl() { return *this; }
	const CTabCtrl& GetTabCtrl() const { return *this; }
	CRect GetUsedArea() const;
	virtual int GetTabItemIndex( size_t nPageIndex ) const { return (int)nPageIndex; }

protected:
	virtual void SetupTabs();
	virtual void ResetTooltips();
	virtual void OnUsedAreaChanged();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg) override;
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	void PostNcDestroy() override;
	afx_msg void OnDestroy();
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchange( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
