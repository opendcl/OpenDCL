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
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyProperty( TPropertyPtr pProp );

public:
	void SetDwgListCtrl( CArxDwgListCtrl* pDwgList ) { mpDwgList = pDwgList; }

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT OnSelchange( WPARAM wParam, LPARAM lParam );
	afx_msg void OnDropdown();
	afx_msg void OnDestroy();	
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
