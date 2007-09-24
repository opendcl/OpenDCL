// ArxCheckBoxCtrl.h : header file
//

#pragma once

#include "CheckBoxCtrl.h"
#include "ArxControlServices.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CArxCheckBoxCtrl window

class CArxCheckBoxCtrl : public CCheckBoxCtrl
{
	CArxControlServices	mArxServices;

// Construction
public:
	CArxCheckBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxCheckBoxCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
