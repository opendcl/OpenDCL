// ArxColorComboBoxCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "ArxControlServices.h"
#include "R2006AcUiMRUComboBoxFixup.h"


/////////////////////////////////////////////////////////////////////////////
// CArxArrowComboBoxCtrl window

class CArxColorComboBoxCtrl : public CAcUiColorComboBox, public CDialogControl
{
	CArxControlServices	mArxServices;

public:
	CArxColorComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxColorComboBoxCtrl();

// DialogControl Interface
public:
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	CRect GetWndRect() const override;
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	bool OnApplyUseVisualStyle( TPropertyPtr pProp ) override; //Prop::UseVisualStyle

	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted

protected:
	virtual void OnListChanged();

protected:
	DECLARE_MESSAGE_MAP();

protected:
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	void PostNcDestroy() override;
	afx_msg LRESULT OnModifyContent( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnResetContent( WPARAM wParam, LPARAM lParam );
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCbnSelchange();
	afx_msg void OnCbnDropdown();
};
