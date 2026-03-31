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
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	CDragDropService* GetDragDropService() override { return &mDragDropService; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT& dropEffect ) override;

protected:
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnCbnSelchange();
	afx_msg void OnDblclk();
	afx_msg void OnKillfocus();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetfocus();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};
