// ArxComboBoxCtrl.h : header file
//

#pragma once

#include "ComboBoxCtrl.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"


/////////////////////////////////////////////////////////////////////////////
// CArxComboBoxCtrl window

class CArxComboBoxCtrl : public CComboBoxCtrl
{
	CArxControlServices	mArxServices;
	CArxDragDropService mDragDropService;

public:
	CArxComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, CComboHandler* pHandler = NULL, bool bCreate = true );
	virtual ~CArxComboBoxCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	CDragDropService* GetDragDropService() override { return &mDragDropService; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;

protected:
	DECLARE_MESSAGE_MAP();

protected:
	afx_msg void OnCbnSelchange();
	afx_msg void OnCbnDropdown();
	afx_msg void OnCbnKillfocus();
	afx_msg void OnCbnSetfocus();
	afx_msg void OnCbnEditchange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
