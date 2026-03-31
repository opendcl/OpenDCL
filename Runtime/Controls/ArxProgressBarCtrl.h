// ArxProgressBarCtrl.h : header file
//

#pragma once

#include "ProgressBarCtrl.h"
#include "ArxControlServices.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CArxProgressBarCtrl window

class CArxProgressBarCtrl : public CProgressBarCtrl
{
	CArxControlServices	mArxServices;

// Construction
public:
	CArxProgressBarCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxProgressBarCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	void ApplyPosition() override; //move control window to new position

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
};
