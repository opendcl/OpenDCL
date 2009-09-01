// ArxImageComboBoxCtrl.h : header file
//

#pragma once

#include "ImageComboBoxCtrl.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"


/////////////////////////////////////////////////////////////////////////////
// CArxImageComboBoxCtrl window

class CArxImageComboBoxCtrl : public CImageComboBoxCtrl
{
	CArxControlServices	mArxServices;
	CArxDragDropService mDragDropService;

public:
	CArxImageComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, CComboHandler* pHandler = NULL, bool bCreate = true );
	virtual ~CArxImageComboBoxCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual CDragDropService* GetDragDropService() { return &mDragDropService; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
	afx_msg void OnDestroy();	
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnEditchange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
