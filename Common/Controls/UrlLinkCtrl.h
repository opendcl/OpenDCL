// UrlLinkCtrl.h : header file
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
// CUrlLinkCtrl window

class CUrlLinkCtrl : public CWnd, public CDialogControl
{
	CAcadColorService mColorService;
	HCURSOR mhHandCursor;
	CFont mFont;

	// Default colors you can change
	// These are global, so they're the same for all links.
	static COLORREF g_colorVisited;

// Construction
public:
	CUrlLinkCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CUrlLinkCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual CString GetWndCaption() const; //get window caption from properties
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
  afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
};
