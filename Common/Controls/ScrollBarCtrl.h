// ScrollBarCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CScrollBarCtrl window

class CScrollBarCtrl : public CScrollBar, public CDialogControl
{
	CAcadColorService mColorService;

public:
	CScrollBarCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CScrollBarCtrl();

protected:
	virtual void OnScroll(UINT nSBCode, UINT nPos);

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	bool OnApplyForegroundColor( TPropertyPtr pProp ) override; //Prop::ForegroundColor
	CAcadColorService* GetColorService() override { return &mColorService; }

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void PostNcDestroy() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void HScroll(UINT nSBCode, UINT nPos);
	afx_msg void VScroll(UINT nSBCode, UINT nPos);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
