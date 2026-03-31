// ArxAngleSlideCtrl.h : header file
//

#pragma once

#include "AngleSlideCtrl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxAngleSlideCtrl window

class CArxAngleSlideCtrl : public CAngleSlideCtrl
{
	CArxControlServices	mArxServices;
	int mnLastReportedPosition;

public:
	CArxAngleSlideCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxAngleSlideCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;

// CRoundSliderCtrl Interface
	void PostMessageToParent( const int nTBCode ) const override;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnOutofmemory(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcapture(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
