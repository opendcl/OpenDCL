// ArxRadioButtonCtrl.h : header file
//

#pragma once

#include "RadioButtonCtrl.h"
#include "ArxControlServices.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CArxRadioButtonCtrl window

class CArxRadioButtonCtrl : public CRadioButtonCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxRadioButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxRadioButtonCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
