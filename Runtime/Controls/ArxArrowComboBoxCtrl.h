// ArxArrowComboBoxCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "ArxControlServices.h"
#include "R2006AcUiMRUComboBoxFixup.h"


/////////////////////////////////////////////////////////////////////////////
// CArxArrowComboBoxCtrl window

class CArxArrowComboBoxCtrl : public CAcUiArrowHeadComboBox, public CDialogControl
{
	CArxControlServices	mArxServices;

public:
	CArxArrowComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxArrowComboBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CRect GetWndRect() const;
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyProperty( TPropertyPtr pProp );
	virtual bool OnApplyUseVisualStyle( TPropertyPtr pProp ); //Prop::UseVisualStyle

protected:
	virtual void OnListChanged();

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	afx_msg LRESULT OnModifyContent( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnResetContent( WPARAM wParam, LPARAM lParam );
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCbnSelchange();
	afx_msg void OnCbnDropdown();
};
