// ArxMonthCtrl.h : header file
//

#pragma once

#include "MonthCtrl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxLabelCtrl window

class CArxMonthCtrl : public CMonthCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxMonthCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxMonthCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnGetdaystate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelect(NMHDR* pNMHDR, LRESULT* pResult);
};
