// ArxListBoxCtrl.h : header file
//

#pragma once

#include "ListBoxCtrl.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"


/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl window

class CArxListBoxCtrl : public CListBoxCtrl
{
	CArxControlServices	mArxServices;
	CArxDragDropService mDragDropService;

public:
	CArxListBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxListBoxCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual CDragDropService* GetDragDropService() { return &mDragDropService; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT dropEffect );

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSelchange();
	afx_msg void OnDblclk();
	afx_msg void OnKillfocus();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetfocus();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
