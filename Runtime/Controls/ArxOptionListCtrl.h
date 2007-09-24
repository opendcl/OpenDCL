// ArxOptionListCtrl.h : header file
//

#pragma once

#include "OptionListCtrl.h"
#include "ArxControlServices.h"
#include "OleOdcDropTarget.h"


/////////////////////////////////////////////////////////////////////////////
// CArxOptionListCtrl window

class CArxOptionListCtrl : public COptionListCtrl
{
	CArxControlServices	mArxServices;

	COleOdcDropTarget m_DropTarget;

// Construction
public:
	CArxOptionListCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxOptionListCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg void OnSelchange();
	afx_msg void OnDblclk();
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
};
