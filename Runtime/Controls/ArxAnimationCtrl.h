// ArxAnimationCtrl.h : header file
//

#pragma once

#include "AnimationCtrl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxAnimationCtrl window

class CArxAnimationCtrl : public CAnimationCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxAnimationCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxAnimationCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
};
