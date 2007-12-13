// ArxTextBoxCtrl.h : header file
//

#pragma once

#include "TextBoxCtrl.h"
#include "ArxControlServices.h"
#include "OleOdcDropTarget.h"


/////////////////////////////////////////////////////////////////////////////
// CArxTextBoxCtrl window

class CArxTextBoxCtrl : public CTextBoxCtrl
{
	CArxControlServices	mArxServices;
	bool mbFocusClick;

	COleOdcDropTarget m_DropTarget;

public:
	CArxTextBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
									 CInputFilter* pFilter = NULL, DWORD dwAcUiStyle = 0, bool bCreate = true );
	virtual ~CArxTextBoxCtrl();

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
	afx_msg void OnChange();
	afx_msg void OnErrspace();
	afx_msg void OnMaxtext();
	afx_msg void OnUpdate();
	afx_msg void OnSetFocus( CWnd* pFocus );
	afx_msg void OnKillFocus( CWnd* pFocus );
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
