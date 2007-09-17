// ArxComboBoxCtrl.h : header file
//

#pragma once

#include "ComboBoxCtrl.h"
#include "ArxControlServices.h"
#include "OleOdcDropTarget.h"


/////////////////////////////////////////////////////////////////////////////
// CArxComboBoxCtrl window

class CArxComboBoxCtrl : public CComboBoxCtrl
{
	CArxControlServices	mArxServices;

	COleOdcDropTarget m_DropTarget;
	bool m_bInvokeWithSendString;

public:
	CArxComboBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, CComboHandler* pHandler = NULL, bool bCreate = true );
	virtual ~CArxComboBoxCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyProperty( TPropertyPtr pProp );

// Operations
public:
	void SetDragnDrop(BOOL bRegister);

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
	afx_msg void OnDestroy();	
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
