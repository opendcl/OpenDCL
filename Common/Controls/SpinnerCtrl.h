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
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	virtual afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
