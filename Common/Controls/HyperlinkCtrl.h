// HyperlinkCtrl.h : header file
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
// CHyperlinkCtrl window

class CHyperlinkCtrl : public CWnd, public CDialogControl
{
	CAcadColorService mColorService;
	HCURSOR mhHandCursor;
	CFont mFont;

	// Default colors you can change
	// These are global, so they're the same for all links.
	static COLORREF g_colorVisited;

// Construction
public:
	CHyperlinkCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CHyperlinkCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	CString GetWndCaption() const override; //get window caption from properties
	bool ApplyProperty( TPropertyPtr pProp ) override;
	CAcadColorService* GetColorService() override { return &mColorService; }

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg) override;
	void PostNcDestroy() override;
  afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
