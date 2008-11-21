// ArxSlideCtrl.h : header file
//

#pragma once

#include "SlideCtrl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxSlideCtrl window

class CArxSlideCtrl : public CSlideCtrl
{
	CArxControlServices	mArxServices;
	int mnLastReportedPosition;

public:
	CArxSlideCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxSlideCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );

	virtual void OnPositionChanged( int nNewPos );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnOutofmemory(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcapture(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
