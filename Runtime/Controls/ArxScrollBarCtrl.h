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
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );

protected:
	virtual void OnScroll(UINT nSBCode, UINT nPos);
};
