// ArxFrameCtrl.h : header file
//

#pragma once

#include "FrameCtrl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxFrameCtrl window

class CArxFrameCtrl : public CFrameCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxFrameCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxFrameCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
};
