// SlideCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CSlideCtrl window

class CSlideCtrl : public CSliderCtrl, public CDialogControl
{
	CAcadColorService mColorService;

public:
	CSlideCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CSlideCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	bool OnApplyBackgroundColor( TPropertyPtr pProp ) override;
	CAcadColorService* GetColorService() override { return &mColorService; }

	virtual void OnPositionChanged( int nNewPos, bool bNotify = true ) {}

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	void PostNcDestroy() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void HScroll(UINT nSBCode, UINT nPos);
	afx_msg void VScroll(UINT nSBCode, UINT nPos);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
