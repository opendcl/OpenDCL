// SpinnerCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CSpinnerCtrl window

class CSpinnerCtrl : public CSpinButtonCtrl, public CDialogControl
{
	CAcadColorService mColorService;

public:
	CSpinnerCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CSpinnerCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	CAcadColorService* GetColorService() override { return &mColorService; }

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	void PostNcDestroy() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
