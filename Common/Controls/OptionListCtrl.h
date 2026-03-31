// OptionListCtrl.h : header file
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
// COptionListCtrl window

class COptionListCtrl : public CListBox, public CDialogControl
{
	CAcadColorService mColorService;
	int mnRowHeight;
	CImageList mImageList;
	bool mbTrackingMouse;
	int idxInitialFocusItem;

// Construction
public:
	COptionListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~COptionListCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	void HandleDpiChanged() override; //handle relayed WM_DPICHANGED_AFTERPARENT message
	CAcadColorService* GetColorService() override { return &mColorService; }

// Operations
public:
	void ResetTooltips();

protected:
	DECLARE_MESSAGE_MAP()

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	void PostNcDestroy() override;
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnLbnSelchange();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
