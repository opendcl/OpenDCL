// ArxScrollBarCtrl.h : header file
//

#pragma once

#include "ScrollBarCtrl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxScrollBarCtrl window

class CArxScrollBarCtrl : public CScrollBarCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxScrollBarCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxScrollBarCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;

protected:
	void OnScroll(UINT nSBCode, UINT nPos) override;
};
