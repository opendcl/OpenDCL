// ArxSpinnerCtrl.h : header file
//

#pragma once

#include "SpinnerCtrl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxSpinnerCtrl window

class CArxSpinnerCtrl : public CSpinnerCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxSpinnerCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxSpinnerCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
