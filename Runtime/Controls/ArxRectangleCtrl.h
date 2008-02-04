// ArxRectangleCtrl.h : header file
//

#pragma once

#include "RectangleCtrl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxRectangleCtrl window

class CArxRectangleCtrl : public CRectangleCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxRectangleCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxRectangleCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
};
