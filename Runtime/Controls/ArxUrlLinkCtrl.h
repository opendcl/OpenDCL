// ArxUrlLinkCtrl.h : header file
//

#pragma once

#include "UrlLinkCtrl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxUrlLinkCtrl window

class CArxUrlLinkCtrl : public CUrlLinkCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxUrlLinkCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxUrlLinkCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
