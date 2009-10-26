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
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual void ApplyPosition(); //move control window to new position

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
};
