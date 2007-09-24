// ArxListBoxCtrl.h : header file
//

#pragma once

#include "ListBoxCtrl.h"
#include "ArxControlServices.h"
#include "OleOdcDropTarget.h"


/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl window

class CArxListBoxCtrl : public CListBoxCtrl
{
	CArxControlServices	mArxServices;

	COleOdcDropTarget m_DropTarget;

public:
	CArxListBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxListBoxCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyProperty( TPropertyPtr pProp );

// Operations
public:
	void GetCurrentSelection();
	void SetDragnDrop(BOOL bRegister);

// Overrides
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg void OnSelchange();
	afx_msg void OnDblclk();
	afx_msg void OnKillfocus();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetfocus();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
};
