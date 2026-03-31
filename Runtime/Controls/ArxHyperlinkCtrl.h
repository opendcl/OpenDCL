// ArxHyperlinkCtrl.h : header file
//

#pragma once

#include "HyperlinkCtrl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxHyperlinkCtrl window

class CArxHyperlinkCtrl : public CHyperlinkCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxHyperlinkCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxHyperlinkCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
