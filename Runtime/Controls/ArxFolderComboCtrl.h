// ArxFolderComboCtrl.h : header file
//

#pragma once

#include "FolderComboCtrl.h"
#include "ArxControlServices.h"

class CArxDwgListCtrl;


/////////////////////////////////////////////////////////////////////////////
// CArxFolderComboCtrl window

class CArxFolderComboCtrl : public CFolderComboCtrl
{
	CArxControlServices	mArxServices;
	CArxDwgListCtrl* mpDwgList;

public:
	CArxFolderComboCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxFolderComboCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	bool ApplyProperty( TPropertyPtr pProp ) override;

public:
	void SetDwgListCtrl( CArxDwgListCtrl* pDwgList ) { mpDwgList = pDwgList; }

protected:
	DECLARE_MESSAGE_MAP();

protected:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnCbnSelchange();
	afx_msg void OnCbnDropdown();
	afx_msg void OnCbnCloseup();
	afx_msg void OnCbnDestroy();	
	afx_msg void OnCbnKillfocus();
	afx_msg void OnCbnSetfocus();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
